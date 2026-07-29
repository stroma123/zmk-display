/*
 * Copyright (c) 2026 stroma
 * SPDX-License-Identifier: MIT
 *
 * Minimal custom widget: a label tracking the active layer via ZMK events.
 * Structure follows zmk/app/src/display/widgets/layer_status.c.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>

#include "layer_label.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct layer_label_state {
    zmk_keymap_layer_index_t index;
    const char *label;
};

static void set_layer_text(lv_obj_t *label, struct layer_label_state state) {
    if (state.label == NULL || strlen(state.label) == 0) {
        lv_label_set_text_fmt(label, LV_SYMBOL_KEYBOARD " %i", state.index);
    } else {
        lv_label_set_text_fmt(label, LV_SYMBOL_KEYBOARD " %s", state.label);
    }
}

static void layer_label_update_cb(struct layer_label_state state) {
    struct zmk_widget_layer_label *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_layer_text(widget->obj, state); }
}

static struct layer_label_state layer_label_get_state(const zmk_event_t *eh) {
    zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();
    return (struct layer_label_state){
        .index = index, .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index))};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_label, struct layer_label_state, layer_label_update_cb,
                            layer_label_get_state)

ZMK_SUBSCRIPTION(widget_layer_label, zmk_layer_state_changed);

int zmk_widget_layer_label_init(struct zmk_widget_layer_label *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    sys_slist_append(&widgets, &widget->node);

    widget_layer_label_init();
    return 0;
}

lv_obj_t *zmk_widget_layer_label_obj(struct zmk_widget_layer_label *widget) {
    return widget->obj;
}
