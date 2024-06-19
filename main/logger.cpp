#include <logger.h>

static SemaphoreHandle_t loggerSemaphore;
static int8_t loggerSkip = false;

static const char *TAG = "[LOGGER]";

static information_logger_t informationLogger = {
    .globalPercent = 0,
    .globalEnergyLeft = 1200000,
    .globalTotalCharged = 0,
    .globalRemainingTime = 0,
    .globalBatteryCurrent = 0,
    .globalBatteryESR = 500,
    .globalBatteryHealth = 0,
    .globalDeviceFirmware = 10,
    .globalDeviceUpTime = 0,
    .globalDeviceUtilization = 0,
    .globalRealTime = 0,
};

static inverter_logger_t inverterLogger = {
    .globalBatteryVoltage = 0,
    .globalBatteryIC = 0,
    .globalBatteryIDC = 0,
    .globalPVPower = 0,
    .globalOutputPower = 0,
    .globalGridVoltage = 0,
    .globalTemperature = 0,
    .globalCurrentMode = LINE_MODE,
    .globalSetMode = LINE_MODE,
};

static inverter_setting_t inverterSetting = {
    .globalCutOffLevel = 25,
    .globalStartLevel = 25,
    .globalExternalVoltage = 5120,
    .globalExternalLevel = 20,
    .globalTotalEnergy = 5000000,
    .globalPVMin = 200,
};

void logger_GetInformation(information_logger_t **informationLogger_)
{
    *(informationLogger_) = &informationLogger;
}

void logger_GetInverterInformation(inverter_logger_t **inverterLogger_)
{
    *(inverterLogger_) = &inverterLogger;
}

void logger_GetInverterSetting(inverter_setting_t **inverterSetting_)
{
    strcpy(inverterSetting.globalSSID, "TheKey_2");
    strcpy(inverterSetting.globalPassword, "");
    strcpy(inverterSetting.globalToken, "CPyX3p3N3AactgHJpY7DOpESq9s9ElSR");
    *(inverterSetting_) = &inverterSetting;
}

static void logger_UpdateInformation(void *pvParameter)
{
    static int32_t engeryExchange = 0;
    static int32_t extraCurrent = 0;
    static int32_t oldVoltage = 0;
    static int32_t oldCurrent = 0;
    static int8_t relayStatus = false;

    informationLogger.globalDeviceUtilization = (100 - lv_task_get_idle()) * 10;
    informationLogger.globalBatteryCurrent = (inverterLogger.globalBatteryIC - inverterLogger.globalBatteryIDC);
    // if (informationLogger.globalBatteryCurrent > 200 || informationLogger.globalBatteryCurrent < -200)
    // {
    //     return;
    // }
    // if (inverterLogger.globalBatteryVoltage > 5800 || inverterLogger.globalBatteryVoltage < 0)
    // {
    //     return;
    // }
    if ((inverterLogger.globalPVPower <= 50) && informationLogger.globalBatteryCurrent <= 0)
    {
        extraCurrent = -1l;
    }
    else
    {
        extraCurrent = 0l;
    }
    engeryExchange = (inverterLogger.globalBatteryVoltage - (informationLogger.globalBatteryCurrent * informationLogger.globalBatteryESR / 100l));
    engeryExchange *= (informationLogger.globalBatteryCurrent + extraCurrent);
    engeryExchange /= 360l;
    informationLogger.globalEnergyLeft += engeryExchange;
    informationLogger.globalTotalCharged += (inverterLogger.globalPVPower * 5) / 18;
    informationLogger.globalDeviceUpTime += 1;
    if (engeryExchange < 0)
    {
        informationLogger.globalRemainingTime = informationLogger.globalEnergyLeft / engeryExchange;
    }
    else if (engeryExchange > 0)
    {
        informationLogger.globalRemainingTime = (inverterSetting.globalTotalEnergy - informationLogger.globalEnergyLeft) / engeryExchange;
    }
    if (inverterLogger.globalBatteryVoltage >= 5680 && informationLogger.globalBatteryCurrent == 0)
    {
        informationLogger.globalBatteryHealth = 100 - ((inverterSetting.globalTotalEnergy - informationLogger.globalEnergyLeft) * 100 / inverterSetting.globalTotalEnergy);
        if (informationLogger.globalBatteryHealth > 100)
            informationLogger.globalBatteryHealth = 100;
        else if (informationLogger.globalBatteryHealth < 0)
            informationLogger.globalBatteryHealth = 0;
        informationLogger.globalEnergyLeft = inverterSetting.globalTotalEnergy;
    }
    if (inverterLogger.globalBatteryVoltage == 5120 && informationLogger.globalBatteryCurrent == 0)
    {
        int32_t checkEnergy = (20 * inverterSetting.globalTotalEnergy / 100);
        if (informationLogger.globalEnergyLeft < checkEnergy)
            informationLogger.globalEnergyLeft = checkEnergy;
    }
    informationLogger.globalPercent = informationLogger.globalEnergyLeft * 100 / inverterSetting.globalTotalEnergy;
    if ((informationLogger.globalRealTime / 3600) % 24 == 0 && (informationLogger.globalRealTime / 60) % 60 == 0)
    {
        informationLogger.globalTotalCharged = 0;
    }
    if (oldVoltage != 0 && oldCurrent != 0)
    {
        if (abs(oldVoltage - inverterLogger.globalBatteryVoltage) > 0050l && abs(oldCurrent - informationLogger.globalBatteryCurrent) > 0)
        {
            informationLogger.globalBatteryESR = (informationLogger.globalBatteryESR * (SAMPLE_RATE_ESR - 1) + abs(oldVoltage - inverterLogger.globalBatteryVoltage) * 100 / abs(oldCurrent - informationLogger.globalBatteryCurrent)) / SAMPLE_RATE_ESR;
        }
    }
    oldVoltage = inverterLogger.globalBatteryVoltage;
    oldCurrent = informationLogger.globalBatteryCurrent;

    if (inverterLogger.globalBatteryVoltage < inverterSetting.globalExternalVoltage && informationLogger.globalBatteryCurrent == 0)
    {
        if (informationLogger.globalEnergyLeft < (inverterSetting.globalExternalLevel * inverterSetting.globalTotalEnergy / 100))
            informationLogger.globalEnergyLeft = (inverterSetting.globalExternalLevel * inverterSetting.globalTotalEnergy / 100);
        if (!relayStatus)
        {
            relay_On();
            relayStatus = true; 
        }
    }
    else if (relayStatus && inverterLogger.globalBatteryVoltage > inverterSetting.globalExternalVoltage)
    {
        relay_Off();
        relayStatus = false;
    }
    informationLogger.globalRealTime++;
}

esp_err_t logger_SendCommandWithSkip(char *data)
{
    uint32_t timer = 0;
    uart_wait_tx_idle_polling(UART_NUM_1);
    if (loggerSkip)
    {
        xSemaphoreGive(loggerSemaphore);
        return ESP_FAIL;
    }
    while (timer < 10)
    {
        if (loggerSkip)
        {
            xSemaphoreGive(loggerSemaphore);
            return ESP_FAIL;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
        timer++;
    }
    timer = 0;
    if (loggerSkip)
    {
        xSemaphoreGive(loggerSemaphore);
        return ESP_FAIL;
    }
    uart_write_bytes(UART_NUM_1, data, strlen(data));
    return ESP_OK;
}

void logger_Task(void *pvParameter)
{
    char *QPIGS = (char *)malloc(256);
    memset(QPIGS, 0, 256);
    memcpy(QPIGS, logger_InverterString("QPIGS"), strlen(logger_InverterString("QPIGS")) + 1);

    // char *START_INVERTER_1 = (char *)malloc(256);
    // memset(START_INVERTER_1, 0, 256);
    // memcpy(START_INVERTER_1, logger_InverterString("PBCV48.0"), strlen(logger_InverterString("PBCV48.0")) + 1);
    // char *START_INVERTER_2 = (char *)malloc(256);
    // memset(START_INVERTER_2, 0, 256);
    // memcpy(START_INVERTER_2, logger_InverterString("PBDV49.0"), strlen(logger_InverterString("PBDV49.0")) + 1);

    // char *STOP_INVERTER_1 = (char *)malloc(256);
    // memset(STOP_INVERTER_1, 0, 256);
    // memcpy(STOP_INVERTER_1, logger_InverterString("PBCV51.0"), strlen(logger_InverterString("PBCV51.0")) + 1);
    // char *STOP_INVERTER_2 = (char *)malloc(256);
    // memset(STOP_INVERTER_2, 0, 256);
    // memcpy(STOP_INVERTER_2, logger_InverterString("PBDV56.0"), strlen(logger_InverterString("PBDV56.0")) + 1);

    // POP01 // Solar
    // POP02 // Battery

    char *START_INVERTER = (char *)malloc(256);
    memset(START_INVERTER, 0, 256);
    memcpy(START_INVERTER, logger_InverterString("POP02"), strlen(logger_InverterString("POP02")) + 1);

    char *STOP_INVERTER = (char *)malloc(256);
    memset(STOP_INVERTER, 0, 256);
    memcpy(STOP_INVERTER, logger_InverterString("POP01"), strlen(logger_InverterString("POP01")) + 1);

    vTaskDelay(pdMS_TO_TICKS(15000));

    while (1)
    {
        if (loggerSkip)
        {
            vTaskDelay(pdMS_TO_TICKS(15000));
            loggerSkip = false;
        }
        xSemaphoreTake(loggerSemaphore, portMAX_DELAY);
        uart_wait_tx_idle_polling(UART_NUM_1);
        if (loggerSkip)
        {
            xSemaphoreGive(loggerSemaphore);
            continue;
        }
        uart_write_bytes(UART_NUM_1, QPIGS, strlen(QPIGS));
        if ((informationLogger.globalPercent > inverterSetting.globalStartLevel) && (inverterLogger.globalPVPower >= inverterSetting.globalPVMin) && inverterLogger.globalCurrentMode == LINE_MODE)
        {
            inverterLogger.globalSetMode = INVERTER_MODE;
            if (logger_SendCommandWithSkip(START_INVERTER) == ESP_FAIL)
            {
                vTaskDelay(pdMS_TO_TICKS(1000));
                inverterLogger.globalCurrentMode = LINE_MODE;
                continue;
            }
        }
        else if ((informationLogger.globalPercent <= inverterSetting.globalCutOffLevel || (inverterLogger.globalBatteryIDC > 30 && informationLogger.globalPercent < 30)) && inverterLogger.globalCurrentMode == INVERTER_MODE)
        {
            inverterLogger.globalSetMode = LINE_MODE;
            if (logger_SendCommandWithSkip(STOP_INVERTER) == ESP_FAIL)
            {
                vTaskDelay(pdMS_TO_TICKS(1000));
                inverterLogger.globalCurrentMode = INVERTER_MODE;
                continue;
            }
        }
        uart_wait_tx_idle_polling(UART_NUM_1);
        xSemaphoreGive(loggerSemaphore);
        vTaskDelay(pdMS_TO_TICKS(2500));
    }
}

void request_Task(void *pvParameter)
{
    uint8_t *requestData = (uint8_t *)malloc(256);
    memset(requestData, 0, 256);
    while (1)
    {
        int len = uart_read_bytes(UART_NUM_2, requestData, (256 - 1), pdMS_TO_TICKS(30));
        if (len == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(30));
            continue;
        }
        // ESP_LOG_BUFFER_HEX("[loggerData]", requestData, 30);
        loggerSkip = true;
        xSemaphoreTake(loggerSemaphore, portMAX_DELAY);
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
            // ESP_LOGI("[responseData]", "len=%d", len);
            // ESP_LOG_BUFFER_HEX("[responseData]", responseData, 200);
            if (responseData[0] == 0x28 && responseData[109] == 0x0D && len == 110)
            {
                if (logger_InverterCRC((char *)responseData, 107) != (uint16_t)((uint16_t)responseData[107] << 8 | responseData[108]))
                    continue;
                inverterLogger.globalBatteryVoltage = (responseData[41] - 0x30) * 1000 + (responseData[42] - 0x30) * 100 + (responseData[44] - 0x30) * 10 + (responseData[45] - 0x30);
                inverterLogger.globalBatteryIC = (responseData[47] - 0x30) * 100 + (responseData[48] - 0x30) * 10 + (responseData[49] - 0x30);
                inverterLogger.globalBatteryIDC = (responseData[77] - 0x30) * 10000 + (responseData[78] - 0x30) * 1000 + (responseData[79] - 0x30) * 100 + (responseData[80] - 0x30) * 10 + (responseData[81] - 0x30);
                inverterLogger.globalPVPower = (responseData[98] - 0x30) * 10000 + (responseData[99] - 0x30) * 1000 + (responseData[100] - 0x30) * 100 + (responseData[101] - 0x30) * 10 + (responseData[102] - 0x30);
                inverterLogger.globalOutputPower = (responseData[28] - 0x30) * 1000 + (responseData[29] - 0x30) * 100 + (responseData[30] - 0x30) * 10 + (responseData[31] - 0x30);
                inverterLogger.globalGridVoltage = (responseData[1] - 0x30) * 1000 + (responseData[2] - 0x30) * 100 + (responseData[3] - 0x30) * 10 + (responseData[5] - 0x30);
                inverterLogger.globalTemperature = (responseData[55] - 0x30) * 1000 + (responseData[56] - 0x30) * 100 + (responseData[57] - 0x30) * 10 + (responseData[58] - 0x30);
                responseData[0] = 0x00;
                responseData[109] = 0x00;
                continue;
            }
            else if (responseData[0] == 0x28 && responseData[1] == 0x41 && responseData[2] == 0x43 && responseData[3] == 0x4B && responseData[4] == 0x39 && responseData[5] == 0x20 && responseData[6] == 0x0D)
            {
                if (inverterLogger.globalSetMode == LINE_MODE)
                    inverterLogger.globalCurrentMode = LINE_MODE;
                else if (inverterLogger.globalSetMode == INVERTER_MODE)
                    inverterLogger.globalCurrentMode = INVERTER_MODE;
                continue;
            }
            else if (responseData[0] == 0x05 && responseData[1] == 0x03 && len == 95)
            {
                responseData[15] = (uint8_t)informationLogger.globalPercent;
                uint16_t crc = ModRTU_CRC(responseData, 93);
                responseData[93] = (crc & 0xFF00) >> 8;
                responseData[94] = (crc & 0x00FF);
            }
            uart_write_bytes(UART_NUM_2, (const char *)responseData, len);
            uart_wait_tx_idle_polling(UART_NUM_2);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
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

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &logger_UpdateInformation,
        .name = "[logger_UpdateInformation]"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000000ull));
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

uint16_t ModRTU_CRC(uint8_t *buf, int len)
{
    uint16_t crc = 0xFFFF;

    for (int pos = 0; pos < len; pos++)
    {
        crc ^= (uint16_t)buf[pos]; // XOR byte into least sig. byte of crc

        for (int i = 8; i != 0; i--)
        { // Loop over each bit
            if ((crc & 0x0001) != 0)
            {              // If the LSB is set
                crc >>= 1; // Shift right and XOR 0xA001
                crc ^= 0xA001;
            }
            else           // Else LSB is not set
                crc >>= 1; // Just shift right
        }
    }
    crc = ((crc & 0x00FF) << 8) | ((crc & 0xFF00) >> 8);
    return crc;
}

int32_t logger_EstimateBatteryPercent(int32_t voltage)
{
    int32_t percent = 0;
    if (voltage > 5430)
    {
        percent = 100;
    }
    else if (voltage < 5120 && voltage > 4000)
    {
        percent = (int32_t)(272.0 - (double)0.1348 * voltage + (double)0.000016741 * voltage * voltage);
    }
    else
        percent = (int32_t)((100.0) * (double)exp(-(voltage - 5430) * (voltage - 5430) / 60000.0));

    if (percent > 100)
        percent = 100;
    else if (percent < 0)
        percent = 0;
    return percent;
}