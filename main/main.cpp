#include <main.h>
#include <ui.h>
#include <logger.h>
#include <relay.h>

SemaphoreHandle_t xGuiSemaphore;
inverter_logger_t *_inverterLogger;
information_logger_t *_informationLogger;

static const char *TAG = "[MAIN]";

void testTask(void *pvParameter)
{
    logger_GetInverterInformation(&_inverterLogger);
    logger_GetInformation(&_informationLogger);
    while (1)
    {
        ui_UpdateBatteryPercent(_informationLogger->globalEnergyLeft * 100 / 5000000);
        ui_UpdateBatteryVoltage(_inverterLogger->globalBatteryVoltage / 10);
        ui_UpdateInformation();
        vTaskDelay(pdMS_TO_TICKS(1500));
    }
}

static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

void guiTask(void *pvParameter)
{
    xGuiSemaphore = xSemaphoreCreateMutex();
    lv_init();
    lvgl_driver_init();
    static lv_color_t buf1[DISP_BUF_SIZE];
    static lv_color_t buf2[DISP_BUF_SIZE];
    static lv_disp_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = touch_driver_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    ui_Init();
    while (1)
    {
        vTaskDelay(1);
        if (xSemaphoreTake(xGuiSemaphore, (TickType_t)10) == pdTRUE)
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
    vTaskDelete(NULL);
}

extern "C" void app_main()
{
    relay_Init();
    relay_On(); // Test only
    logger_InitUART();
    printf("\r\nAPP %s is start!~\r\n", TAG);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    xTaskCreatePinnedToCore(guiTask, "[guiTask]", 4096 * 2, NULL, 0, NULL, 1);
    xTaskCreate(testTask, "[testTask]", 4096, NULL, 0, NULL);
}