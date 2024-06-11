#pragma once

#ifndef NVS_H
#define NVS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esp_log.h>
#include <esp_err.h>

#include <nvs_flash.h>
#include <logger.h>

#define STORAGE_NAMESPACE "storage"

void nvs_ReadInverterSetting(inverter_setting_t *inverterSetting);
void nvs_WriteInverterSetting(inverter_setting_t *inverterSetting);

#endif