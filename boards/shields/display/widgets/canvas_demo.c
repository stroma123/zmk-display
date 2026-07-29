/*
 * Copyright (c) 2026 stroma
 * SPDX-License-Identifier: MIT
 *
 * Static canvas test: exercises the draw primitives the status widgets
 * use (rect, line, arc, text) on an LVGL canvas buffer.
 */

#include <zephyr/kernel.h>

#include "canvas_demo.h"

int zmk_widget_canvas_demo_init(struct zmk_widget_canvas_demo *widget, lv_obj_t *parent) {
    widget->obj = lv_canvas_create(parent);
    lv_canvas_set_buffer(widget->obj, widget->cbuf, ZMK_CANVAS_DEMO_SIZE, ZMK_CANVAS_DEMO_SIZE,
                         LV_IMG_CF_TRUE_COLOR);
    lv_canvas_fill_bg(widget->obj, lv_color_hex(0x202020), LV_OPA_COVER);

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_opa = LV_OPA_TRANSP;
    rect_dsc.border_color = lv_color_hex(0xFFD800); // yellow
    rect_dsc.border_width = 2;
    rect_dsc.radius = 6;
    lv_canvas_draw_rect(widget->obj, 2, 2, 44, 44, &rect_dsc);

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_hex(0x00FFFF); // cyan
    line_dsc.width = 2;
    lv_point_t pts[2] = {{6, 42}, {42, 6}};
    lv_canvas_draw_line(widget->obj, pts, 2, &line_dsc);

    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);
    arc_dsc.color = lv_color_hex(0xFF00FF); // magenta
    arc_dsc.width = 3;
    lv_canvas_draw_arc(widget->obj, 24, 24, 14, 0, 270, &arc_dsc);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_white();
    label_dsc.font = &lv_font_montserrat_26;
    lv_canvas_draw_text(widget->obj, 4, 10, 44, &label_dsc, "C");

    return 0;
}

lv_obj_t *zmk_widget_canvas_demo_obj(struct zmk_widget_canvas_demo *widget) { return widget->obj; }
