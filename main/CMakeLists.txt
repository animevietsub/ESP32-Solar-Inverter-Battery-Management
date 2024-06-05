FILE(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

idf_component_register(SRCS ${SOURCES}
                    INCLUDE_DIRS .
                    REQUIRES lvgl_esp32_drivers lvgl_touch lvgl_tft lvgl c_image c_font)

target_compile_definitions(${COMPONENT_LIB} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)
