#pragma once

#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esp_system.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_err.h>

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_freertos_hooks.h>
#include <freertos/semphr.h>
#include <freertos/event_groups.h>

#include <lvgl/lvgl.h>
#include <lvgl_helpers.h>

#define LV_TICK_PERIOD_MS 10

void testTask(void *pvParameter);
static void lv_tick_task(void *arg);
void guiTask(void *pvParameter); 

#endif