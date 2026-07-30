/*
 * Copyright (c) 2021 Pete Johanson
 * SPDX-License-Identifier: MIT
 *
 * Bongo cat animation driven by WPM, ported from the archived mlego
 * variant. Frames are stored sideways (40x128); the screen rotates the
 * lv_img 90 degrees clockwise at render time instead of the archived
 * software canvas rotation. Unlike the original, WPM events are
 * marshalled through the display work queue (ZMK_DISPLAY_WIDGET_LISTENER)
 * instead of touching LVGL from the event thread.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/wpm.h>

#include "bongo_cat.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

enum anim_state {
    anim_state_none,
    anim_state_idle,
    anim_state_slow,
    anim_state_fast,
};

static enum anim_state current_anim_state = anim_state_none;
static const void **images;

LV_IMG_DECLARE(idle_img1);
LV_IMG_DECLARE(idle_img2);
LV_IMG_DECLARE(idle_img3);
LV_IMG_DECLARE(idle_img4);
LV_IMG_DECLARE(idle_img5);

LV_IMG_DECLARE(slow_img);

LV_IMG_DECLARE(fast_img1);
LV_IMG_DECLARE(fast_img2);

static const void *idle_images[] = {
    &idle_img1, &idle_img2, &idle_img3, &idle_img4, &idle_img5,
};

static const void *fast_images[] = {
    &fast_img1,
    &fast_img2,
};

struct bongo_cat_state {
    uint8_t wpm;
};

static void set_img_src(void *var, int32_t val) {
    lv_obj_t *img = (lv_obj_t *)var;
    // all frames share one size, so position and transform are unaffected
    lv_img_set_src(img, images[val]);
}

static void set_anim_state(struct zmk_widget_bongo_cat *widget, uint8_t wpm) {
    if (wpm < CONFIG_DISPLAY_BONGO_CAT_IDLE_LIMIT) {
        if (current_anim_state != anim_state_idle) {
            lv_anim_init(&widget->anim);
            lv_anim_set_var(&widget->anim, widget->obj);
            lv_anim_set_time(&widget->anim, 200);
            lv_anim_set_values(&widget->anim, 0, 4);
            lv_anim_set_exec_cb(&widget->anim, set_img_src);
            lv_anim_set_repeat_count(&widget->anim, 10);
            lv_anim_set_repeat_delay(&widget->anim, 100);
            images = idle_images;
            current_anim_state = anim_state_idle;
            lv_anim_start(&widget->anim);
        }
    } else if (wpm < CONFIG_DISPLAY_BONGO_CAT_SLOW_LIMIT) {
        if (current_anim_state != anim_state_slow) {
            lv_anim_del(widget->obj, set_img_src);
            lv_img_set_src(widget->obj, &slow_img);
            current_anim_state = anim_state_slow;
        }
    } else {
        if (current_anim_state != anim_state_fast) {
            lv_anim_init(&widget->anim);
            lv_anim_set_var(&widget->anim, widget->obj);
            lv_anim_set_time(&widget->anim, 200);
            lv_anim_set_values(&widget->anim, 0, 1);
            lv_anim_set_exec_cb(&widget->anim, set_img_src);
            lv_anim_set_repeat_count(&widget->anim, LV_ANIM_REPEAT_INFINITE);
            lv_anim_set_repeat_delay(&widget->anim, 200);
            images = fast_images;
            current_anim_state = anim_state_fast;
            lv_anim_start(&widget->anim);
        }
    }
}

static void bongo_cat_update_cb(struct bongo_cat_state state) {
    struct zmk_widget_bongo_cat *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_anim_state(widget, state.wpm); }
}

static struct bongo_cat_state bongo_cat_get_state(const zmk_event_t *eh) {
    return (struct bongo_cat_state){.wpm = zmk_wpm_get_state()};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_bongo_cat, struct bongo_cat_state, bongo_cat_update_cb,
                            bongo_cat_get_state)
ZMK_SUBSCRIPTION(widget_bongo_cat, zmk_wpm_state_changed);

int zmk_widget_bongo_cat_init(struct zmk_widget_bongo_cat *widget, lv_obj_t *parent) {
    widget->obj = lv_img_create(parent);

    sys_slist_append(&widgets, &widget->node);

    set_anim_state(widget, 0);

    widget_bongo_cat_init();
    return 0;
}

lv_obj_t *zmk_widget_bongo_cat_obj(struct zmk_widget_bongo_cat *widget) { return widget->obj; }
