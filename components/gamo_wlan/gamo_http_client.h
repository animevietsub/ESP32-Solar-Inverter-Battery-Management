#pragma once

#ifndef GAMO_HTTP_CLIENT_H
#define GAMO_HTTP_CLIENT_H

#include <sys/param.h>
#include <esp_http_client.h>
#include <esp_tls.h>
#include <string>

#define MAX_HTTP_OUTPUT_BUFFER 2048

class GAMO_HTTP_CLIENT
{
public:
    esp_err_t POST_JSON(std::string data_, char **return_data_ = nullptr);
    esp_err_t POST_FORM(std::string data_, char **return_data_ = nullptr);
    esp_err_t POST_FILE(std ::string file_name_, std::string name_, uint8_t *file_data_, uint32_t file_size_, std::string other_data_ = "", char **return_data_ = nullptr);
    GAMO_HTTP_CLIENT(std::string host_, std::string path_, bool disable_auto_redirect_ = true);
    ~GAMO_HTTP_CLIENT();

protected:
    esp_http_client_config_t config;
    std::string send_data;
    char *http_data;
    esp_http_client_handle_t http_client;
    uint16_t buffer_size;
};

esp_err_t _http_event_handler(esp_http_client_event_t *evt);

#endif