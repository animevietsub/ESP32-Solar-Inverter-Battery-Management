#include <logger.h>

static SemaphoreHandle_t loggerSemaphore;

static const char *TAG = "[LOGGER]";

static information_logger_t informationLogger = {
    .globalEnergyLeft = 3000000,
    .globalTotalCharged = 0,
    .globalRemainingTime = 0,
    .globalBatteryCurrent = 0,
    .globalBatteryESR = 0,
    .globalBatteryHealth = 0,
    .globalDeviceFirmware = 0,
    .globalDeviceUpTime = 0,
    .globalDeviceUtilization = 0,
};

static inverter_logger_t inverterLogger = {
    .globalBatteryVoltage = 0,
    .globalBatteryIC = 0,
    .globalBatteryIDC = 0,
    .globalInverterStatus = 0x00,
};

void logger_GetInformation(information_logger_t **informationLogger_)
{
    *(informationLogger_) = &informationLogger;
}

void logger_GetInverterInformation(inverter_logger_t **inverterLogger_)
{
    *(inverterLogger_) = &inverterLogger;
}

void logger_UpdateInformation(void *pvParameter)
{
    static int32_t EngeryExchange = 0;
    while (1)
    {
        informationLogger.globalDeviceUtilization = (100 - lv_task_get_idle()) * 10;
        EngeryExchange = ((inverterLogger.globalBatteryIC - inverterLogger.globalBatteryIDC) * inverterLogger.globalBatteryVoltage / 360);
        informationLogger.globalEnergyLeft += EngeryExchange;
        informationLogger.globalTotalCharged = EngeryExchange; // Test
        informationLogger.globalBatteryCurrent = (inverterLogger.globalBatteryIC - inverterLogger.globalBatteryIDC) * 10;
        if (EngeryExchange != 0)
        {
            informationLogger.globalRemainingTime = informationLogger.globalEnergyLeft / EngeryExchange;
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // check tick for more ac
    }
}

void logger_Task(void *pvParameter)
{
    char *QPIGS = (char *)malloc(256);
    memset(QPIGS, 0, 256);
    memcpy(QPIGS, logger_InverterString("QPIGS"), strlen(logger_InverterString("QPIGS")) + 1);
    vTaskDelay(pdMS_TO_TICKS(10000));
    while (1)
    {
        xSemaphoreTake(loggerSemaphore, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(10));
        uart_wait_tx_idle_polling(UART_NUM_1);
        uart_write_bytes(UART_NUM_1, QPIGS, strlen(QPIGS));
        xSemaphoreGive(loggerSemaphore);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void request_Task(void *pvParameter)
{
    uint8_t *requestData = (uint8_t *)malloc(256);
    memset(requestData, 0, 256);
    while (1)
    {
        int len = uart_read_bytes(UART_NUM_2, requestData, (256 - 1), pdMS_TO_TICKS(50));
        if (len == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        ESP_LOG_BUFFER_HEX("[loggerData]", requestData, 30);
        xSemaphoreTake(loggerSemaphore, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(10));
        uart_wait_tx_idle_polling(UART_NUM_1);
        uart_write_bytes(UART_NUM_1, (const char *)requestData, len);
        xSemaphoreGive(loggerSemaphore);
    }
}

void response_Task(void *pvParameter)
{
    uint8_t *responseData = (uint8_t *)malloc(1024);
    memset(responseData, 0, 1024);
    while (1)
    {
        int len = uart_read_bytes(UART_NUM_1, responseData, (1024 - 1), pdMS_TO_TICKS(100));
        if (len > 0)
        {
            ESP_LOGI("[responseData]", "len=%d", len);
            ESP_LOG_BUFFER_HEX("[responseData]", responseData, 200);
            uart_write_bytes(UART_NUM_2, (const char *)responseData, len);
            uart_wait_tx_idle_polling(UART_NUM_2);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void logger_InitUART()
{
    loggerSemaphore = xSemaphoreCreateMutex();
    uart_config_t uart_config = {
        .baud_rate = 2400,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 1024 * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, GPIO_NUM_22, GPIO_NUM_27, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, 1024 * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, GPIO_NUM_17, GPIO_NUM_16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    xTaskCreate(request_Task, "[request_Task]", 4096, NULL, 0, NULL);
    xTaskCreate(response_Task, "[response_Task]", 4096, NULL, 0, NULL);
    xTaskCreate(logger_Task, "[logger_Task]", 4096, NULL, 0, NULL);

    xTaskCreate(logger_UpdateInformation, "[logger_UpdateInformation]", 4096, NULL, 0, NULL);
}

char *logger_InverterString(char *data)
{
    static char buffer[50];
    uint16_t crc = logger_InverterCRC(data, strlen(data));
    sprintf(buffer, "%s%c%c\r", data, uint8_t(crc >> 8), uint8_t(crc & 0x00FF));
    return buffer;
}

uint16_t logger_InverterCRCUpdate(uint16_t crc, uint8_t data)
{
    int i;
    crc = crc ^ ((uint16_t)data << 8);
    for (i = 0; i < 8; i++)
    {
        if (crc & 0x8000)
            crc = (crc << 1) ^ 0x1021;
        else
            crc <<= 1;
    }
    return crc;
}

uint16_t logger_InverterCRC(char *data, int len)
{
    uint16_t x = 0;
    while (len--)
    {
        x = logger_InverterCRCUpdate(x, (uint16_t)*data++);
    }
    return (x);
}