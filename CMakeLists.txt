cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
list(APPEND EXTRA_COMPONENT_DIRS components/lvgl_esp32_drivers components/lvgl_esp32_drivers/lvgl_touch components/lvgl_esp32_drivers/lvgl_tft)

if (NOT DEFINED PROJECT_NAME)
	project(ESP32_INVERTER_BATTERY)
endif (NOT DEFINED PROJECT_NAME)
spiffs_create_partition_image(storage data FLASH_IN_PROJECT)

