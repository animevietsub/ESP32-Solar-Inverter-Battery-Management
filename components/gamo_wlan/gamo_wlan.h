#pragma once

#ifndef GAMO_WLAN_H
#define GAMO_WLAN_H

#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <lwip/err.h>
#include <lwip/sys.h>

void gamo_wifi_init_sta(void);
void gamo_wifi_connect(const char *ssid, const char *password, TaskFunction_t callbackFunction);

#endif