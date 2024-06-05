#pragma once

#ifndef LOGGER_H
#define LOGGER_H

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
#include <driver/uart.h>
#include <esp_freertos_hooks.h>
#include <freertos/semphr.h>
#include <freertos/event_groups.h>

#include <lvgl/lvgl.h>
#include <lvgl_helpers.h>

typedef struct _information_logger_t
{
    int32_t globalEnergyLeft;
    int32_t globalTotalCharged;
    int32_t globalRemainingTime;
    int32_t globalBatteryCurrent;
    uint32_t globalBatteryESR;
    uint16_t globalBatteryHealth;
    int32_t globalDeviceFirmware;
    int32_t globalDeviceUpTime;
    uint16_t globalDeviceUtilization;
} information_logger_t;

typedef struct _inverter_logger_t
{
    int16_t globalBatteryVoltage;
    int16_t globalBatteryIC;
    int16_t globalBatteryIDC;
    uint8_t globalInverterStatus;
} inverter_logger_t;

void logger_GetInformation(information_logger_t **informationLogger_);
void logger_GetInverterInformation(inverter_logger_t **inverterLogger_);
void logger_UpdateInformation();
void logger_InitUART();
char *logger_InverterString(char *data);
uint16_t logger_InverterCRCUpdate(uint16_t crc, uint8_t data);
uint16_t logger_InverterCRC(char *data, int len);

#endif