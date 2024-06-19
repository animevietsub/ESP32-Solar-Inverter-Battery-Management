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
#include <esp_timer.h>

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/uart.h>
#include <esp_freertos_hooks.h>
#include <freertos/semphr.h>
#include <freertos/event_groups.h>

#include <lvgl/lvgl.h>
#include <lvgl_helpers.h>

#include <sdkconfig.h>
#include <math.h>

#include <relay.h>

#define SAMPLE_RATE_ESR 10

#define LINE_MODE 0
#define INVERTER_MODE 1

typedef struct _information_logger_t
{
    int32_t globalPercent;
    int32_t globalEnergyLeft;
    int32_t globalTotalCharged;
    int32_t globalRemainingTime;
    int32_t globalBatteryCurrent;
    int32_t globalBatteryESR;
    int32_t globalBatteryHealth;
    int32_t globalDeviceFirmware;
    uint32_t globalDeviceUpTime;
    uint16_t globalDeviceUtilization;
    int64_t globalRealTime;
} information_logger_t;

typedef struct _inverter_logger_t
{
    int32_t globalBatteryVoltage;
    int32_t globalBatteryIC;
    int32_t globalBatteryIDC;
    int32_t globalPVPower;
    int32_t globalOutputPower;
    int32_t globalGridVoltage;
    int32_t globalTemperature;
    int32_t globalCurrentMode;
    int32_t globalSetMode;
} inverter_logger_t;

typedef struct _inverter_setting_t
{
    int32_t globalCutOffLevel;
    int32_t globalStartLevel;
    int32_t globalExternalVoltage;
    int32_t globalExternalLevel;
    int32_t globalTotalEnergy;
    int32_t globalPVMin;
    char globalSSID[50];
    char globalPassword[50];
    char globalToken[50];
} inverter_setting_t;

void logger_GetInformation(information_logger_t **informationLogger_);
void logger_GetInverterInformation(inverter_logger_t **inverterLogger_);
void logger_GetInverterSetting(inverter_setting_t **inverterSetting_);
void logger_UpdateInformation();
void logger_InitUART();
char *logger_InverterString(char *data);
uint16_t logger_InverterCRCUpdate(uint16_t crc, uint8_t data);
uint16_t logger_InverterCRC(char *data, int len);
uint16_t ModRTU_CRC(uint8_t *buf, int len);
int32_t logger_EstimateBatteryPercent(int32_t voltage);

#endif