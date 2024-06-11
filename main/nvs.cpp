#include <nvs.h>

static const char *TAG = "[NVS]";

void nvs_ReadInverterSetting(inverter_setting_t *inverterSetting)
{
    nvs_handle_t nvs_handle;
    size_t nvs_size = 0;
    ESP_ERROR_CHECK(nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle));
    ESP_LOGI(TAG, "Reading saved setting...");
    nvs_get_blob(nvs_handle, "inv_setting", NULL, &nvs_size);
    if (nvs_size != sizeof(inverter_setting_t))
    {
        nvs_close(nvs_handle);
        ESP_LOGI(TAG, "No saved setting found!, writing current setting. size = %d", nvs_size);
        nvs_WriteInverterSetting(inverterSetting);
        return;
    }
    nvs_get_blob(nvs_handle, "inv_setting", inverterSetting, &nvs_size);
    nvs_close(nvs_handle);
}

void nvs_WriteInverterSetting(inverter_setting_t *inverterSetting)
{
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle));
    ESP_LOGI(TAG, "Writing setting...");
    nvs_set_blob(nvs_handle, "inv_setting", inverterSetting, sizeof(inverter_setting_t));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}