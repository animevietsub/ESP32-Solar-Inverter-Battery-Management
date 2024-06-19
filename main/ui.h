#pragma once

#ifndef UI_H
#define UI_H

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

#include <nvs.h>

void ui_Init();
void ui_HomeInit();
void ui_SettingInit();
void ui_UpdateBatteryPercent(int32_t percent);
void ui_UpdateBatteryVoltage(int32_t voltage);
void ui_UpdateInformation();
void ui_UpdateTime();

#endif