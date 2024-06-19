#pragma once

#ifndef MQTT_H
#define MQTT_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esp_system.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_err.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"

#define BROKER_URI "mqtt://sgp1.blynk.cloud"
#define AUTH_TOKEN "CPyX3p3N3AactgHJpY7DOpESq9s9ElSR"

void mqtt_Init(char *auth_token);
void mqtt_Start(void *pvParameter);
void mqtt_Publish(const char *topic, const char *data);


#endif