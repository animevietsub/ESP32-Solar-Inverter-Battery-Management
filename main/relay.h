#pragma once

#ifndef RELAY_H
#define RELAY_H

#include <esp_log.h>
#include <esp_err.h>
#include <driver/ledc.h>

void relay_On();
void relay_Off();
void relay_Init();

#endif