#include "forecast.h"
#include "user_data.h"
#include "wifi.h"

#include <float.h>

#include <zephyr/kernel.h>
#include <zephyr/data/json.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/timeutil.h>
#include <zephyr/posix/time.h>

LOG_MODULE_REGISTER(forecast);

K_MSGQ_DEFINE(forecast_data_msgq, sizeof(struct forecast_data_full), 1, 1);

static unsigned char recv_buf[3072];

static int forecast_socket_setup(const char* server, const char* port, int *sock);
static void forecast_response_cb(struct http_response *response, enum http_final_call final_data, void *user_data);

static int forecast_socket_setup(const char* server, const char* port, int *sock)
{
    int ret = 0;

    struct zsock_addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    ret = zsock_getaddrinfo(server, port, &hints, &res);
    if (ret != 0) {
        LOG_ERR("getaddrinfo status: %d\n", ret);
        return -EINVAL;
    }

    *sock = zsock_socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (*sock < 0) {
        LOG_ERR("Failed to create HTTP socket - %d.", -errno);
        zsock_freeaddrinfo(res);
        return -errno;
    }

    ret = zsock_connect(*sock, res->ai_addr, res->ai_addrlen);
    if (ret < 0) {
        LOG_ERR("Cannot connect to remote - %d.", -errno);
        zsock_shutdown(*sock, SHUT_RDWR);
        zsock_close(*sock);
        *sock = -1;
        zsock_freeaddrinfo(res);
        return -errno;
    }

    zsock_freeaddrinfo(res);
    return 0;
}

int forecast_get(const char *server, const char *api_key)
{
    int sock = -1;
    struct sockaddr_in addr;
    int timeout = 5 * MSEC_PER_SEC;

    int ret = forecast_socket_setup(server, "80", &sock);
    if (sock < 0) {
        LOG_ERR("Failed to establish HTTP connection.");
    } else {
        LOG_INF("HTTP connection to %s established", FORECAST_SERVER);
    }

    struct http_request req;
    memset(&req, 0, sizeof(req));

    req.method = HTTP_GET;
    req.url = api_key;
    req.host = server;
    req.protocol = "HTTP/1.1";
    req.response = forecast_response_cb;
    req.recv_buf = recv_buf;
    req.recv_buf_len = sizeof(recv_buf);

    ret = http_client_req(sock, &req, timeout, NULL); 

    zsock_shutdown(sock, SHUT_RDWR);
    zsock_close(sock);

    return ret;
}

static void forecast_response_cb(struct http_response *response, enum http_final_call final_data, void *user_data)
{
    if (final_data == HTTP_DATA_MORE) {
        LOG_INF("Partial data received (%zd bytes).", response->data_len);
    } else if (final_data == HTTP_DATA_FINAL) {
        LOG_INF("All the data received (%zd bytes).", response->data_len);
    }

    LOG_INF("GET response status - %s.", response->http_status);

    forecast_response_parse(strchr(response->recv_buf, '{'));
}

int forecast_response_parse(char *response)
{
    cJSON_Hooks hooks = {
        .malloc_fn = malloc,
        .free_fn = free,
    };

    cJSON_InitHooks(&hooks);

    cJSON *json = cJSON_Parse(response);
    if (json == NULL) {
        LOG_ERR("Error parsing JSON");
        return -1;
    }

    cJSON *current = cJSON_GetObjectItemCaseSensitive(json, "current");
    cJSON *temperature_current = cJSON_GetObjectItemCaseSensitive(current, "temperature_2m");
    cJSON *humidity_current = cJSON_GetObjectItemCaseSensitive(current, "relative_humidity_2m");
    cJSON *wind_speed_current = cJSON_GetObjectItemCaseSensitive(current, "wind_speed_10m");

    cJSON *hourly = cJSON_GetObjectItemCaseSensitive(json, "hourly");
    cJSON *temperature_array = cJSON_GetObjectItemCaseSensitive(hourly, "temperature_2m");
    cJSON *humidity_array = cJSON_GetObjectItemCaseSensitive(hourly, "relative_humidity_2m");
    cJSON *wind_speed_array = cJSON_GetObjectItemCaseSensitive(hourly, "wind_speed_10m");
    cJSON *uvi_array = cJSON_GetObjectItemCaseSensitive(hourly, "uv_index");

    struct timespec tspec;
    struct tm *real_time;
    clock_gettime(CLOCK_REALTIME, &tspec);
    real_time = gmtime(&tspec.tv_sec);

    struct forecast_data_full forecast;
    forecast_array_get_min_max(temperature_array, &forecast.temperature);
    forecast_array_get_min_max(humidity_array, &forecast.humidity);
    forecast_array_get_min_max(wind_speed_array, &forecast.wind_speed);
    forecast_array_get_min_max(uvi_array, &forecast.uvi);

    if (cJSON_IsNumber(temperature_current))
        forecast.temperature.current = temperature_current->valuedouble;
    else
        LOG_ERR("Error parsing current temperature");

    if (cJSON_IsNumber(humidity_current))
        forecast.humidity.current = humidity_current->valuedouble;
    else
        LOG_ERR("Error parsing current humidity");

    forecast_array_get_current(wind_speed_array, &forecast.wind_speed, real_time->tm_hour);
    forecast_array_get_current(uvi_array, &forecast.uvi, real_time->tm_hour);

    while (k_msgq_put(&forecast_data_msgq, &forecast, K_NO_WAIT) != 0) {
        k_msgq_purge(&forecast_data_msgq);
    }

    cJSON_Delete(json);

    return 0;
}

int forecast_array_get_min_max(cJSON *array, struct forecast_data *data)
{
    if (!cJSON_IsArray(array)) {
        LOG_ERR("Not an array");
        return -1;
    }

    double min = FLT_MAX, max = -FLT_MAX;

    for (size_t i = 0; i < 24; i++) {
        if (cJSON_GetArrayItem(array, i)->valuedouble < min)
            min = cJSON_GetArrayItem(array, i)->valuedouble;
        if (cJSON_GetArrayItem(array, i)->valuedouble > max)
            max = cJSON_GetArrayItem(array, i)->valuedouble;
    }

    data->max = max;
    data->min = min;

    return 0;
}

int forecast_array_get_current(cJSON *array, struct forecast_data *data, unsigned int current_hour)
{
    if (!cJSON_IsArray(array)) {
        LOG_ERR("Not an array");
        return -1;
    }

    data->current = cJSON_GetArrayItem(array, current_hour)->valuedouble;

    return 0;
}


int forecast_sntp_sync(void) {
    int ret;
	struct sntp_time now;
	struct timespec tspec;

	ret = sntp_simple(SNTP_SERVER, 1000, &now);
    if (ret != 0) {
        LOG_ERR("SNTP sync error - %d code", ret);
        return -1;
    }
        
    tspec.tv_sec = now.seconds;
    tspec.tv_nsec = ((uint64_t)now.fraction * (1000lu * 1000lu * 1000lu)) >> 32;

    clock_settime(CLOCK_REALTIME, &tspec);

    LOG_INF("Acquired time from NTP server: %u", (uint32_t)tspec.tv_sec);

    return ret;
}

