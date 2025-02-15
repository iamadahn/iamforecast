#include "widgets.h"

static lv_obj_t* label_create(lv_obj_t *parent,
                              char *text,
                              const lv_font_t *font,
                              lv_color_t color,
                              lv_align_t align,
                              unsigned short x,
                              unsigned short y)
{
    lv_obj_t* label;
    label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_align(label, align, x, y);

    return label;
}

static lv_obj_t* line_create(lv_obj_t *parent,
                             lv_point_t *points,
                             lv_style_t *style,
                             lv_color_t color,
                             unsigned char width,
                             unsigned char points_num,
                             lv_align_t align,
                             unsigned int x,
                             unsigned int y)
{
    lv_style_set_line_width(style, width);
    lv_style_set_line_rounded(style, true);

    lv_obj_t* line;
    line = lv_line_create(parent);
    lv_line_set_points(line, points, points_num);
    lv_obj_add_style(line, style, 0);
    lv_obj_align(line, align, x, y);

    return line;
}

void frame_widget_create(struct frame_widget *widget,
                                unsigned int width,
                                unsigned int height,
                                unsigned int x_start,
                                unsigned int y_start)
{
    widget->self = lv_obj_create(widget->parent);
    widget->width = width;
    widget->height = height;
    lv_obj_align(widget->self, widget->align, x_start, y_start);
    lv_obj_set_width(widget->self, widget->width);
    lv_obj_set_height(widget->self, widget->height);
    lv_obj_set_style_border_width(widget->self, 0, 0);
    lv_obj_set_style_outline_width(widget->self, 0, 0);
    lv_obj_set_style_outline_pad(widget->self, 0, 0);
    lv_obj_set_style_pad_top(widget->self, 0, 0);
    lv_obj_set_style_pad_left(widget->self, 0, 0);
    lv_obj_set_style_radius(widget->self, 0, 0);
    lv_obj_set_style_bg_color(widget->self, widget->bg_color, 0);
    lv_obj_clear_flag(widget->self, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(widget->self, widget->event_cb, widget->event_code, NULL);

    unsigned char label_opacity = 5;
    unsigned char label_pixel_size = strlen(widget->text) * JETBRAINS_14_WIDTH;
    unsigned int label_x_start = DEFAULT_OUTLINE + (label_opacity * 2), label_y_start = 5;

    lv_obj_t *label = label_create(
        widget->self,
        widget->text,
        widget->font,
        widget->font_color,
        widget->align,
        label_x_start,
        label_y_start
    );

    unsigned short line_x_start = label_x_start - label_opacity,
        line_x_end = label_x_start + label_pixel_size + label_opacity,
        line_y_start = DEFAULT_OUTLINE,
        line_y_end = height - DEFAULT_OUTLINE;

    widget->line_points[0].x = line_x_start;
    widget->line_points[0].y = line_y_start;

    widget->line_points[1].x = DEFAULT_OUTLINE;
    widget->line_points[1].y = line_y_start;

    widget->line_points[2].x = DEFAULT_OUTLINE;
    widget->line_points[2].y = line_y_end;

    widget->line_points[3].x = width - DEFAULT_OUTLINE;
    widget->line_points[3].y = line_y_end;

    widget->line_points[4].x = width - DEFAULT_OUTLINE;
    widget->line_points[4].y = line_y_start;

    widget->line_points[5].x = line_x_end;
    widget->line_points[5].y = line_y_start;

    lv_style_init(&widget->style);
    lv_obj_t *line = line_create(
        widget->self,
        widget->line_points,
        &widget->style,
        widget->font_color,
        2,
        6,
        widget->align,
        0,
        0
    );
} 

void data_widget_create(struct data_widget *widget,
                               unsigned int width,
                               unsigned int height,
                               unsigned int x_start,
                               unsigned int y_start)
{
    widget->self = lv_obj_create(widget->parent);
    widget->width = width;
    widget->height = height;
    lv_obj_align(widget->self, LV_ALIGN_TOP_LEFT, x_start, y_start);
    lv_obj_set_width(widget->self, widget->width);
    lv_obj_set_height(widget->self, widget->height);
    lv_obj_set_style_border_width(widget->self, 0, 0);
    lv_obj_set_style_outline_width(widget->self, 0, 0);
    lv_obj_set_style_outline_pad(widget->self, 0, 0);
    lv_obj_set_style_pad_top(widget->self, 0, 0);
    lv_obj_set_style_pad_left(widget->self, 0, 0);
    lv_obj_set_style_radius(widget->self, 0, 0);
    lv_obj_set_style_bg_color(widget->self, widget->bg_color, 0);
    lv_obj_clear_flag(widget->self, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(widget->self, widget->event_cb, widget->event_code, NULL);

    /*-----------*/
    /* Main line */
    /*-----------*/
    unsigned char main_line_x_length = widget->width,
        main_line_y_length = widget->height;

    unsigned int main_line_x_start = 0,
        main_line_x_end = main_line_x_length,
        main_line_x_center = main_line_x_length / 2,

        main_line_y_start = (main_line_y_length / 2) + (DEFAULT_OUTLINE / 2),
        main_line_y_end = main_line_y_length;

    widget->main_line_points[0].x = main_line_x_start;
    widget->main_line_points[0].y = main_line_y_start;

    widget->main_line_points[1].x = main_line_x_end;
    widget->main_line_points[1].y = main_line_y_start;

    widget->main_line_points[2].x = main_line_x_center;
    widget->main_line_points[2].y = main_line_y_start;

    widget->main_line_points[3].x = main_line_x_center;
    widget->main_line_points[3].y = main_line_y_end;

    lv_style_init(&widget->style);
    lv_obj_t *main_line = line_create(
        widget->self,
        widget->main_line_points,
        &widget->style,
        widget->font_color,
        1,
        4,
        LV_ALIGN_TOP_LEFT,
        0,
        0
    );

    /*------------*/
    /* Text label */
    /*------------*/
    unsigned char label_pixel_size = strlen(widget->text) * JETBRAINS_12_WIDTH;
    unsigned char label_space_length = 5;
    unsigned char label_opacity = 2;

    unsigned int text_label_x = main_line_x_start + label_space_length + label_opacity,
        text_label_y = 0;

    lv_obj_t *text_label = label_create(
        widget->self,
        widget->text,
        widget->label_font,
        widget->font_color,
        LV_ALIGN_TOP_LEFT,
        text_label_x,
        text_label_y
    );

    /*-----------*/
    /* Left line */
    /*-----------*/
    unsigned int left_line_x_start = text_label_x - label_opacity,
        left_line_y_start = text_label_y + 4;

    widget->left_line_points[0].x = left_line_x_start;
    widget->left_line_points[0].y = left_line_y_start;

    widget->left_line_points[1].x = main_line_x_start;
    widget->left_line_points[1].y = left_line_y_start;

    widget->left_line_points[2].x = main_line_x_start;
    widget->left_line_points[2].y = left_line_y_start + label_space_length;

    lv_obj_t *left_out_line = line_create(
        widget->self,
        widget->left_line_points,
        &widget->style,
        widget->font_color,
        1,
        3,
        LV_ALIGN_TOP_LEFT,
        0,
        0
    );

    /*------------*/
    /* Right line */
    /*------------*/
    unsigned int right_line_x_start = text_label_x + label_pixel_size + label_opacity,
        right_line_y_start = text_label_y + 4;

    widget->right_line_points[0].x = right_line_x_start;
    widget->right_line_points[0].y = right_line_y_start;

    widget->right_line_points[1].x = main_line_x_end - 1;
    widget->right_line_points[1].y = right_line_y_start;

    widget->right_line_points[2].x = main_line_x_end - 1;
    widget->right_line_points[2].y = right_line_y_start + label_space_length;

    lv_obj_t *right_out_line = line_create(
        widget->self,
        widget->right_line_points,
        &widget->style,
        widget->font_color,
        1,
        3,
        LV_ALIGN_TOP_LEFT,
        0,
        0
    );

    /*--------------------*/
    /* Current data label */
    /*--------------------*/
    char* data_label_text = "test";
    widget->data_current_label = label_create(
        widget->self,
        data_label_text,
        widget->data_font,
        widget->font_color,
        LV_ALIGN_CENTER,
        5,
        0
    );

    /*----------------*/
    /* Min data label */
    /*----------------*/
    widget->data_min_label = label_create(
        widget->self,
        data_label_text,
        widget->label_font,
        widget->font_color,
        LV_ALIGN_CENTER,
        -10,
        25
    );

    /*----------------*/
    /* Max data label */
    /*----------------*/
    widget->data_max_label = label_create(
        widget->self,
        data_label_text,
        widget->label_font,
        widget->font_color,
        LV_ALIGN_CENTER,
        23,
        25
    );
}

void data_min_widget_create(struct data_min_widget *widget,
                                   unsigned int width,
                                   unsigned int height,
                                   unsigned int x_start,
                                   unsigned int y_start)
{
    widget->self = lv_obj_create(widget->parent);
    widget->width = width;
    widget->height = height;
    lv_obj_align(widget->self, LV_ALIGN_TOP_LEFT, x_start, y_start);
    lv_obj_set_width(widget->self, widget->width);
    lv_obj_set_height(widget->self, widget->height);
    lv_obj_set_style_border_width(widget->self, 0, 0);
    lv_obj_set_style_outline_width(widget->self, 0, 0);
    lv_obj_set_style_outline_pad(widget->self, 0, 0);
    lv_obj_set_style_pad_top(widget->self, 0, 0);
    lv_obj_set_style_pad_left(widget->self, 0, 0);
    lv_obj_set_style_radius(widget->self, 0, 0);
    lv_obj_set_style_bg_color(widget->self, widget->bg_color, 0);
    lv_obj_clear_flag(widget->self, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(widget->self, widget->event_cb, widget->event_code, NULL);

    /*------------*/
    /* Text label */
    /*------------*/
    unsigned char label_pixel_size = strlen(widget->text) * JETBRAINS_12_WIDTH;
    unsigned char label_space_length = 5;
    unsigned char label_opacity = 2;

    unsigned int text_label_x = label_space_length + label_opacity,
        text_label_y = 0;

    lv_style_init(&widget->style);

    lv_obj_t *text_label = label_create(
        widget->self,
        widget->text,
        widget->label_font,
        widget->font_color,
        LV_ALIGN_TOP_LEFT,
        text_label_x,
        text_label_y
    );

    /*-----------*/
    /* Left line */
    /*-----------*/
    unsigned int left_line_x_start = text_label_x - label_opacity,
        left_line_y_start = text_label_y + 4,
        left_line_x_end = 0;

    widget->left_line_points[0].x = left_line_x_start;
    widget->left_line_points[0].y = left_line_y_start;

    widget->left_line_points[1].x = left_line_x_end;
    widget->left_line_points[1].y = left_line_y_start;

    widget->left_line_points[2].x = left_line_x_end;
    widget->left_line_points[2].y = left_line_y_start + label_space_length;

    lv_obj_t *left_out_line = line_create(
        widget->self,
        widget->left_line_points,
        &widget->style,
        widget->font_color,
        1,
        3,
        LV_ALIGN_TOP_LEFT,
        0,
        0
    );

    /*------------*/
    /* Right line */
    /*------------*/
    unsigned int right_line_x_start = text_label_x + label_pixel_size + label_opacity,
        right_line_y_start = text_label_y + 4,
        right_line_x_end = widget->width - 1;

    widget->right_line_points[0].x = right_line_x_start;
    widget->right_line_points[0].y = right_line_y_start;

    widget->right_line_points[1].x = right_line_x_end;
    widget->right_line_points[1].y = right_line_y_start;

    widget->right_line_points[2].x = right_line_x_end;
    widget->right_line_points[2].y = right_line_y_start + label_space_length;

    lv_obj_t *right_out_line = line_create(
        widget->self,
        widget->right_line_points,
        &widget->style,
        widget->font_color,
        1,
        3,
        LV_ALIGN_TOP_LEFT,
        0,
        0
    );

    /*-------------*/
    /* Bottom line */
    /*-------------*/
    unsigned int bottom_line_x_start = left_line_x_end,
        bottom_line_x_end = right_line_x_end,
        bottom_line_y_start = widget->height - 1;

    widget->bottom_line_points[0].x = left_line_x_end;
    widget->bottom_line_points[0].y = bottom_line_y_start;

    widget->bottom_line_points[1].x = right_line_x_end;
    widget->bottom_line_points[1].y = bottom_line_y_start;

    lv_obj_t *bottom_out_line = line_create(
        widget->self,
        widget->bottom_line_points,
        &widget->style,
        widget->font_color,
        1,
        2,
        LV_ALIGN_TOP_LEFT,
        0,
        0
    );

    /*------------*/
    /* Data label */
    /*------------*/
    char* data_label_text = "0";
    unsigned int data_label_x = 8,
                data_label_y = 15;

    widget->data_label = label_create(
        widget->self,
        data_label_text,
        widget->data_font,
        widget->font_color,
        LV_ALIGN_CENTER,
        data_label_x,
        data_label_y
    );
}

void time_and_date_widget_create(struct time_and_date_widget *widget,
                                        unsigned int width,
                                        unsigned int height,
                                        unsigned int x_start,
                                        unsigned int y_start)
{
    widget->self = lv_obj_create(widget->parent);
    widget->width = width;
    widget->height = height;
    lv_obj_align(widget->self, LV_ALIGN_TOP_LEFT, x_start, y_start);
    lv_obj_set_width(widget->self, widget->width);
    lv_obj_set_height(widget->self, widget->height);
    lv_obj_set_style_border_width(widget->self, 0, 0);
    lv_obj_set_style_outline_width(widget->self, 0, 0);
    lv_obj_set_style_outline_pad(widget->self, 0, 0);
    lv_obj_set_style_pad_top(widget->self, 0, 0);
    lv_obj_set_style_pad_left(widget->self, 0, 0);
    lv_obj_set_style_radius(widget->self, 0, 0);
    lv_obj_set_style_bg_color(widget->self, widget->bg_color, 0);
    lv_obj_clear_flag(widget->self, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(widget->self, widget->event_cb, widget->event_code, NULL);

    /*------------*/
    /* Time label */
    /*------------*/
    char* time_label_text = "13:37";

    widget->time_label = label_create(
        widget->self,
        time_label_text,
        widget->time_font,
        widget->font_color,
        LV_ALIGN_TOP_MID,
        5,
        5
    );

    /*------------*/
    /* Date label */
    /*------------*/
    char* date_label_text = "28.06.2021";

    widget->date_label = label_create(
        widget->self,
        date_label_text,
        widget->date_font,
        widget->font_color,
        LV_ALIGN_TOP_MID,
        5,
        35
    );

    /*-----------*/
    /* Day label */
    /*-----------*/
    char* day_label_text = "Saturday";

    widget->day_of_week_label = label_create(
        widget->self,
        day_label_text,
        widget->day_of_week_font,
        widget->font_color,
        LV_ALIGN_TOP_MID,
        5,
        60
    );
}

void data_widget_update(struct data_widget *widget, struct widget_data data, bool raw_value)
{
    if (widget == NULL)
        return;

    char buf[32];

    if (raw_value) {
        sprintf(buf, "%.1f", data.current);
        lv_label_set_text(widget->data_current_label, buf);
        sprintf(buf, "%.1f", data.max);
        lv_label_set_text(widget->data_max_label, buf);
        sprintf(buf, "%.1f", data.min);
        lv_label_set_text(widget->data_min_label, buf);
    } else {
        sprintf(buf, "%d", (int)data.current);
        lv_label_set_text(widget->data_current_label, buf);
        sprintf(buf, "%d", (int)data.max);
        lv_label_set_text(widget->data_max_label, buf);
        sprintf(buf, "%d", (int)data.min);
        lv_label_set_text(widget->data_min_label, buf);
    }
}

void data_min_widget_update(struct data_min_widget *widget, double value)
{
    if (widget == NULL)
        return;

    char buf[32];

    sprintf(buf, "%.1f", value);
    lv_label_set_text(widget->data_label, buf);
}

void time_and_date_widget_update(struct time_and_date_widget *widget, struct tm *time)
{
    char buf[32];
    sprintf(buf, "%02d:%02d", time->tm_hour, time->tm_min);
    lv_label_set_text(widget->time_label, buf);
    sprintf(buf, "%02d.%02d.%d", time->tm_mday, time->tm_mon + 1, time->tm_year + 1900);
    lv_label_set_text(widget->date_label, buf);
    
    switch (time->tm_wday) {
        case 0:
        lv_label_set_text(widget->day_of_week_label, "Sunday");
        break;

        case 1:
        lv_label_set_text(widget->day_of_week_label, "Monday");
        break;

        case 2:
        lv_label_set_text(widget->day_of_week_label, "Tuesday");
        break;

        case 3:
        lv_label_set_text(widget->day_of_week_label, "Wednesday");
        break;

        case 4:
        lv_label_set_text(widget->day_of_week_label, "Thursday");
        break;

        case 5:
        lv_label_set_text(widget->day_of_week_label, "Friday");
        break;

        case 6:
        lv_label_set_text(widget->day_of_week_label, "Saturday");
        break;
    }
}
