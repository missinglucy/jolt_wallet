#ifndef __JOLT_GUI_STATUSBAR_H__
#define __JOLT_GUI_STATUSBAR_H__

#include "lvgl/lvgl.h"

enum {
    JOLT_GUI_STATUSBAR_INDEX_BATTERY = 0,
    JOLT_GUI_STATUSBAR_INDEX_WIFI,
    JOLT_GUI_STATUSBAR_INDEX_BLUETOOTH,
    JOLT_GUI_STATUSBAR_INDEX_LOCK,
    JOLT_GUI_STATUSBAR_INDEX_NUM,
};

#ifndef CONFIG_JOLT_GUI_STATUSBAR_H
    #define CONFIG_JOLT_GUI_STATUSBAR_H 12
#endif

void statusbar_create();

lv_obj_t *statusbar_get_label();

#endif
