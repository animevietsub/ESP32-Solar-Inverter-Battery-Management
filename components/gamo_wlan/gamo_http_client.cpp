#include <stdio.h>
#include <gamo_http_client.h>

#include <string>
#include <esp_log.h>

static const char *TAG = "[GAMO_HTTP_CLIENT]";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer; // Buffer to store response of http request from event handler
    static int output_len;      // Stores number of bytes read
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        // Clean the buffer in case of a new request
        if (output_len == 0 && evt->user_data)
        {
            // we are just starting to copy the output data into the use
            memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
        }
        /*
         *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
         *  However, event handler can also be used in case chunked encoding is used.
         */
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            // If user_data buffer is configured, copy the response into the buffer
            int copy_len = 0;
            if (evt->user_data)
            {
                // The last byte in evt->user_data is kept for the NULL character in case of out-of-bound access.
                copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
                if (copy_len)
                {
                    memcpy(evt->user_data + output_len, evt->data, copy_len);
                }
            }
            else
            {
                int content_len = esp_http_client_get_content_length(evt->client);
                if (output_buffer == NULL)
                {
                    // We initialize output_buffer with 0 because it is used by strlen() and similar functions therefore should be null terminated.
                    output_buffer = (char *)calloc(content_len + 1, sizeof(char));
                    output_len = 0;
                    if (output_buffer == NULL)
                    {
                        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                copy_len = MIN(evt->data_len, (content_len - output_len));
                if (copy_len)
                {
                    memcpy(output_buffer + output_len, evt->data, copy_len);
                }
            }
            output_len += copy_len;
        }

        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        if (output_buffer != NULL)
        {
            // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
            // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        int mbedtls_err = 0;
        esp_err_t err = esp_tls_get_and_clear_last_error((esp_tls_error_handle_t)evt->data, &mbedtls_err, NULL);
        if (err != 0)
        {
            ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
            ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
        }
        if (output_buffer != NULL)
        {
            free(output_buffer);
            output_buffer = NULL;
        }
        output_len = 0;
        break;
    }
    return ESP_OK;
}

esp_err_t GAMO_HTTP_CLIENT::POST_JSON(std::string data_, char **return_data_)
{
    esp_http_client_set_method(http_client, HTTP_METHOD_POST);
    esp_http_client_set_header(http_client, "Content-Type", "application/json");
    esp_http_client_set_post_field(http_client, data_.c_str(), data_.length());
    esp_err_t err = esp_http_client_perform(http_client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(http_client),
                 esp_http_client_get_content_length(http_client));
        if (return_data_ != nullptr)
        {
            *return_data_ = http_data;
        }
    }
    else
    {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        esp_http_client_close(http_client);
    }
    return err;
}

esp_err_t GAMO_HTTP_CLIENT::POST_FORM(std::string data_, char **return_data_)
{
    esp_http_client_set_method(http_client, HTTP_METHOD_POST);
    esp_http_client_set_header(http_client, "Content-Type", "application/x-www-form-urlencoded");
    esp_http_client_set_post_field(http_client, data_.c_str(), data_.length());
    esp_err_t err = esp_http_client_perform(http_client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(http_client),
                 esp_http_client_get_content_length(http_client));
        if (return_data_ != nullptr)
        {
            *return_data_ = http_data;
        }
    }
    else
    {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        esp_http_client_close(http_client);
    }
    return err;
}

esp_err_t GAMO_HTTP_CLIENT::POST_FILE(std ::string file_name_, std::string name_, uint8_t *file_data_, uint32_t file_size_, std::string other_data_, char **return_data_)
{
    esp_http_client_set_method(http_client, HTTP_METHOD_POST);
    esp_http_client_set_header(http_client, "Content-Type", "multipart/form-data; boundary=GAMO");
    this->send_data.clear();
    this->send_data.append("--GAMO\r\nContent-Disposition: form-data; name=\"");
    this->send_data.append(name_);
    this->send_data.append("\"; filename=\"");
    this->send_data.append(file_name_);
    this->send_data.append("\"\r\nContent-Type: image/jpeg\r\n\r\n");
    for (uint32_t i = 0; i < file_size_; i++)
        this->send_data += *(file_data_ + i);
    while (other_data_.length() > 0)
    {
        std::string name = "";
        std::string value = "";
        uint32_t start_name = 0;
        uint32_t end_name = 1;
        end_name = other_data_.find("=", end_name);
        uint32_t start_value = end_name + 1;
        uint32_t end_value = start_value + 1;
        end_value = other_data_.find("&", end_value);
        name = other_data_.substr(start_name, end_name);
        value = other_data_.substr(start_value, end_value - start_value);
        this->send_data.append("\r\n--GAMO\r\nContent-Disposition: form-data; name=\"");
        this->send_data.append(name);
        this->send_data.append("\"\r\n\r\n");
        this->send_data.append(value);
        if (end_value == -1)
        {
            break;
        }
        other_data_ = other_data_.substr(end_value + 1);
    }
    this->send_data.append("\r\n--GAMO--\r\n");
    esp_http_client_set_post_field(http_client, this->send_data.c_str(), this->send_data.length());
    esp_err_t err = esp_http_client_perform(http_client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(http_client),
                 esp_http_client_get_content_length(http_client));
        if (return_data_ != nullptr)
        {
            *return_data_ = http_data;
        }
    }
    else
    {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
        esp_http_client_close(http_client);
    }
    return err;
}

GAMO_HTTP_CLIENT::GAMO_HTTP_CLIENT(std::string host_, std::string path_, bool disable_auto_redirect_)
{
    http_data = new char[MAX_HTTP_OUTPUT_BUFFER];
    if (http_data == nullptr)
        ESP_LOGE(TAG, "Not enough memory for HTTP_OUTPUT_BUFFER");
    config.host = host_.c_str();
    config.path = path_.c_str();
    config.event_handler = _http_event_handler;
    config.user_data = http_data;
    config.disable_auto_redirect = {disable_auto_redirect_};
    http_client = esp_http_client_init(&config);
}

GAMO_HTTP_CLIENT::~GAMO_HTTP_CLIENT()
{
    delete http_data;
    esp_http_client_cleanup(http_client);
}
