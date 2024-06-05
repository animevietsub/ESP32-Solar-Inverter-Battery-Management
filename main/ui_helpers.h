#pragma once

#ifndef UI_HELPERS_H
#define UI_HELPERS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esp_system.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_err.h>

#include <lvgl/lvgl.h>
#include <lvgl_helpers.h>

void ui_AnimSetOpa(lv_obj_t *obj, int32_t value);
void ui_AnimSetXMove(lv_obj_t *obj, int32_t value);
void ui_IconShowAnimation(lv_obj_t *obj, int delay);
void ui_InfoShowAnimation(lv_obj_t *obj, int delay);
lv_obj_t *ui_RollerCreate(lv_obj_t *parent, lv_coord_t w, lv_coord_t h, lv_obj_t *base, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs, lv_border_side_t border_side, const char *label_text, const char *options_text);

#endif