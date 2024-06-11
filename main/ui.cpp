#include <main.h>
#include <ui.h>
#include <ui_helpers.h>
#include <logger.h>

static const char *TAG = "[UI]";

LV_IMG_DECLARE(ui_img_bg);
LV_IMG_DECLARE(ui_img_cbg);
LV_IMG_DECLARE(ui_img_battery);
LV_IMG_DECLARE(ui_img_voltage);
LV_IMG_DECLARE(ui_img_flash);
LV_IMG_DECLARE(ui_img_lotus);
LV_IMG_DECLARE(ui_img_select);
LV_IMG_DECLARE(ui_img_line);
LV_IMG_DECLARE(ui_img_corner);
LV_FONT_DECLARE(ui_font_percent);
LV_FONT_DECLARE(ui_font_medium);
LV_FONT_DECLARE(ui_font_big);
LV_FONT_DECLARE(ui_font_small);
LV_FONT_DECLARE(ui_font_icon);

uint8_t ui_HomeInfoType = 0;

lv_obj_t *screen_Home;
lv_obj_t *screen_Setting;
lv_draw_mask_line_param_t battery_linemask;
lv_draw_mask_line_param_t voltage_linemask;
lv_objmask_mask_t *battery_objmask;
lv_objmask_mask_t *voltage_objmask;
lv_obj_t *batteryMask;
lv_obj_t *voltageMask;
lv_obj_t *batteryPercent;
lv_obj_t *batteryPercentShadow;
lv_obj_t *flashImg;
lv_obj_t *batteryVoltage;
lv_obj_t *buttonEnergy;
lv_obj_t *buttonBattery;
lv_obj_t *buttonDevice;
lv_obj_t *buttonCornerClose;
lv_obj_t *buttonCornerSetting;
lv_obj_t *selectImg;
lv_obj_t *viewInformation;
lv_obj_t *labelInformation;
lv_obj_t *labelInfo1;
lv_obj_t *valueInfo1;
lv_obj_t *unitInfo1;
lv_obj_t *labelInfo2;
lv_obj_t *valueInfo2;
lv_obj_t *unitInfo2;
lv_obj_t *labelInfo3;
lv_obj_t *valueInfo3;
lv_obj_t *unitInfo3;
lv_obj_t *labelTime;
lv_obj_t *labelTimeEx;
lv_obj_t *rollerCutOffLevel;
lv_obj_t *rollerStartLevel;
lv_obj_t *rollerExternalVoltage;
lv_obj_t *rollerTotalEnergy;
information_logger_t *informationLogger;

void ui_UpdateTime()
{
    if (lv_disp_get_scr_act(NULL) == NULL)
        return;
    char stringData[20];
    if ((informationLogger->globalRealTime / 3600) % 24 <= 12)
    {
        sprintf(stringData, "%02lld:%02lld ", (informationLogger->globalRealTime / 3600) % 24, (informationLogger->globalRealTime / 60) % 60);
        lv_label_set_text(labelTimeEx, "AM ");
    }
    else if ((informationLogger->globalRealTime / 3600) % 24 > 12)
    {
        sprintf(stringData, "%02lld:%02lld ", ((informationLogger->globalRealTime / 3600) % 24) - 12, (informationLogger->globalRealTime / 60) % 60);
        lv_label_set_text(labelTimeEx, "PM ");
    }
    lv_label_set_text(labelTime, stringData);
}

void ui_UpdateInformation()
{
    if (lv_disp_get_scr_act(NULL) == NULL)
        return;
    else if (lv_disp_get_scr_act(NULL) != screen_Home)
        return;
    char stringData[20];
    if (ui_HomeInfoType == 0)
    {
        lv_label_set_text(labelInformation, "Energy Information ");
        lv_label_set_text(labelInfo1, "Energy left ");
        sprintf(stringData, "%d.%03d ", informationLogger->globalEnergyLeft / 1000 / 1000, informationLogger->globalEnergyLeft / 1000 % 1000);
        lv_label_set_text(valueInfo1, stringData);
        lv_label_set_text(unitInfo1, "kWh ");
        lv_label_set_text(labelInfo2, "Total charged ");
        sprintf(stringData, "%d.%03d ", informationLogger->globalTotalCharged / 1000 / 1000, informationLogger->globalTotalCharged / 1000 % 1000);
        lv_label_set_text(valueInfo2, stringData);
        lv_label_set_text(unitInfo2, "kWh ");
        lv_label_set_text(labelInfo3, "Remaining time ");
        sprintf(stringData, "%+d:%02d:%02d ", informationLogger->globalRemainingTime / 60 / 60, abs(informationLogger->globalRemainingTime / 60 % 60), abs(informationLogger->globalRemainingTime % 60));
        lv_label_set_text(valueInfo3, stringData);
        lv_label_set_text(unitInfo3, " ");
        lv_obj_realign(unitInfo1);
        lv_obj_realign(unitInfo2);
        lv_obj_realign(unitInfo3);
    }
    if (ui_HomeInfoType == 1)
    {
        lv_label_set_text(labelInformation, "Battery Information ");
        lv_label_set_text(labelInfo1, "Battery current ");
        sprintf(stringData, "%+d.%d ", informationLogger->globalBatteryCurrent, 0);
        lv_label_set_text(valueInfo1, stringData);
        lv_label_set_text(unitInfo1, "A ");
        lv_label_set_text(labelInfo2, "Battery ESR ");
        sprintf(stringData, "%d.%d ", informationLogger->globalBatteryESR / 10, informationLogger->globalBatteryESR % 10);
        lv_label_set_text(valueInfo2, stringData);
        lv_label_set_text(unitInfo2, "mΩ ");
        lv_label_set_text(labelInfo3, "Battery health ");
        sprintf(stringData, "%d.%d ", informationLogger->globalBatteryHealth / 10, informationLogger->globalBatteryHealth % 10);
        lv_label_set_text(valueInfo3, stringData);
        lv_label_set_text(unitInfo3, "% ");
        lv_obj_realign(unitInfo1);
        lv_obj_realign(unitInfo2);
        lv_obj_realign(unitInfo3);
    }
    if (ui_HomeInfoType == 2)
    {
        lv_label_set_text(labelInformation, "Device Information ");
        lv_label_set_text(labelInfo1, "Device firmware ");
        sprintf(stringData, "%d.%d ", informationLogger->globalDeviceFirmware / 10, informationLogger->globalDeviceFirmware % 10);
        lv_label_set_text(valueInfo1, stringData);
        lv_label_set_text(unitInfo1, "Beta ");
        lv_label_set_text(labelInfo2, "Device up time ");
        sprintf(stringData, "%d.%d ", informationLogger->globalDeviceUpTime / 3600, informationLogger->globalDeviceUpTime % 3600 / 360);
        lv_label_set_text(valueInfo2, stringData);
        lv_label_set_text(unitInfo2, "h ");
        lv_label_set_text(labelInfo3, "Device utilization ");
        sprintf(stringData, "%d.%d ", informationLogger->globalDeviceUtilization / 10, informationLogger->globalDeviceUtilization % 10);
        lv_label_set_text(valueInfo3, stringData);
        lv_label_set_text(unitInfo3, "% ");
        lv_obj_realign(unitInfo1);
        lv_obj_realign(unitInfo2);
        lv_obj_realign(unitInfo3);
    }
}

void ui_MenuClickEvent(lv_obj_t *obj, lv_event_t event)
{
    lv_obj_set_state(obj, LV_STATE_DEFAULT);
    if (event == LV_EVENT_CLICKED)
    {
        if (obj == buttonEnergy)
        {
            lv_obj_align(selectImg, NULL, LV_ALIGN_IN_BOTTOM_MID, 65, 0);
            ui_IconShowAnimation(selectImg, 0);
            ui_InfoShowAnimation(viewInformation, 0);
            ui_HomeInfoType = 0;
            ui_UpdateInformation();
        }
        if (obj == buttonBattery)
        {
            lv_obj_align(selectImg, NULL, LV_ALIGN_IN_BOTTOM_MID, 100, 0);
            ui_IconShowAnimation(selectImg, 0);
            ui_InfoShowAnimation(viewInformation, 0);
            ui_HomeInfoType = 1;
            ui_UpdateInformation();
        }
        if (obj == buttonDevice)
        {
            lv_obj_align(selectImg, NULL, LV_ALIGN_IN_BOTTOM_MID, 135, 0);
            ui_IconShowAnimation(selectImg, 0);
            ui_InfoShowAnimation(viewInformation, 0);
            ui_HomeInfoType = 2;
            ui_UpdateInformation();
        }
    }
}

void ui_CornerClickEvent(lv_obj_t *obj, lv_event_t event)
{
    lv_obj_set_state(obj, LV_STATE_DEFAULT);
    if (event == LV_EVENT_CLICKED)
    {
        if (obj == buttonCornerClose)
        {
            lv_disp_load_scr(screen_Home);
        }
        if (obj == buttonCornerSetting)
        {
            lv_disp_load_scr(screen_Setting);
        }
    }
}

void ui_RollerEvent(lv_obj_t *obj, lv_event_t event)
{
    lv_obj_set_state(obj, LV_STATE_DEFAULT);
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        if (obj == rollerCutOffLevel)
        {
        }
        else if (obj == rollerStartLevel)
        {
        }
        else if (obj == rollerExternalVoltage)
        {
        }
        else if (obj == rollerTotalEnergy)
        {
        }
    }
}

void ui_UpdateBatteryPercent(int32_t percent)
{
    if (lv_disp_get_scr_act(NULL) == NULL)
        return;
    else if (lv_disp_get_scr_act(NULL) != screen_Home)
        return;
    if (percent > 99)
        percent = 99;
    if (percent < 0)
        percent = 0;
    lv_draw_mask_line_points_init(&battery_linemask, 0, 200 - percent * 2, 108, 200 - percent * 2, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
    lv_objmask_update_mask(batteryMask, battery_objmask, &battery_linemask);
    char txtPercent[10];
    sprintf(txtPercent, "%02d ", percent);
    lv_label_set_text(batteryPercent, txtPercent);
    lv_label_set_text(batteryPercentShadow, txtPercent);
}

void ui_UpdateBatteryVoltage(int32_t voltage)
{
    if (lv_disp_get_scr_act(NULL) == NULL)
        return;
    else if (lv_disp_get_scr_act(NULL) != screen_Home)
        return;
    int32_t percent = voltage * 25 / 22 - 545;
    if (percent > 100)
        percent = 100;
    if (percent < 0)
        percent = 0;
    lv_draw_mask_line_points_init(&voltage_linemask, 0, 200 - percent * 2, 108, 200 - percent * 2, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
    lv_objmask_update_mask(voltageMask, voltage_objmask, &voltage_linemask);
    char txtVoltage[25];
    sprintf(txtVoltage, "%02d.%01d#94aeb4 V ", voltage / 10, voltage % 10);
    lv_label_set_text(batteryVoltage, txtVoltage);
}

void ui_HomeInit()
{
    screen_Home = lv_obj_create(NULL, NULL);
    logger_GetInformation(&informationLogger);
    ESP_LOGI(TAG, "Screen Home init!");

    lv_obj_t *backgroundImg = lv_img_create(screen_Home, NULL);
    lv_img_set_src(backgroundImg, &ui_img_bg);
    lv_obj_align(backgroundImg, NULL, LV_ALIGN_CENTER, 0, 0);

    voltageMask = lv_objmask_create(screen_Home, NULL);
    lv_obj_set_size(voltageMask, 94, 207);
    lv_obj_align(voltageMask, NULL, LV_ALIGN_CENTER, -92, 0);
    lv_draw_mask_line_points_init(&voltage_linemask, 0, 200, 94, 200, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
    voltage_objmask = lv_objmask_add_mask(voltageMask, &voltage_linemask);

    lv_obj_t *voltageImg = lv_img_create(voltageMask, NULL);
    lv_img_set_src(voltageImg, &ui_img_voltage);
    lv_obj_align(voltageImg, NULL, LV_ALIGN_CENTER, 0, 0);

    batteryMask = lv_objmask_create(screen_Home, NULL);
    lv_obj_set_size(batteryMask, 108, 215);
    lv_obj_align(batteryMask, NULL, LV_ALIGN_CENTER, -50, 0);
    lv_draw_mask_line_points_init(&battery_linemask, 0, 200, 108, 200, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
    battery_objmask = lv_objmask_add_mask(batteryMask, &battery_linemask);

    lv_obj_t *batteryImg = lv_img_create(batteryMask, NULL);
    lv_img_set_src(batteryImg, &ui_img_battery);
    lv_obj_align(batteryImg, NULL, LV_ALIGN_CENTER, 0, 0);

    batteryPercentShadow = lv_label_create(screen_Home, NULL);
    lv_label_set_text(batteryPercentShadow, "32 ");
    lv_obj_set_style_local_text_color(batteryPercentShadow, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xD56920));
    lv_obj_set_style_local_text_font(batteryPercentShadow, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_percent);
    lv_obj_align(batteryPercentShadow, NULL, LV_ALIGN_CENTER, 15, 0);

    batteryPercent = lv_label_create(screen_Home, NULL);
    lv_label_set_text(batteryPercent, "32 ");
    lv_obj_set_style_local_text_color(batteryPercent, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFE69C));
    lv_obj_set_style_local_text_font(batteryPercent, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_percent);
    lv_obj_align(batteryPercent, NULL, LV_ALIGN_CENTER, 10, 0);

    lv_obj_t *labelBatteryLevel = lv_label_create(screen_Home, NULL);
    lv_label_set_text(labelBatteryLevel, "Battery level ");
    lv_obj_set_style_local_text_color(labelBatteryLevel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x94AEB4));
    lv_obj_set_style_local_text_font(labelBatteryLevel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_medium);
    lv_obj_align(labelBatteryLevel, NULL, LV_ALIGN_CENTER, 0, -45);

    lv_obj_t *labelPercentLeft = lv_label_create(screen_Home, NULL);
    lv_label_set_text(labelPercentLeft, "% left ");
    lv_obj_set_style_local_text_color(labelPercentLeft, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x94AEB4));
    lv_obj_set_style_local_text_font(labelPercentLeft, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_medium);
    lv_obj_align(labelPercentLeft, NULL, LV_ALIGN_CENTER, 25, 45);

    flashImg = lv_img_create(screen_Home, NULL);
    lv_img_set_src(flashImg, &ui_img_flash);
    lv_obj_align(flashImg, NULL, LV_ALIGN_CENTER, -144, 108);

    batteryVoltage = lv_label_create(screen_Home, NULL);
    lv_label_set_text(batteryVoltage, "0.0#94aeb4 V ");
    lv_label_set_recolor(batteryVoltage, "true");
    lv_obj_set_style_local_text_color(batteryVoltage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(batteryVoltage, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_big);
    lv_obj_align(batteryVoltage, NULL, LV_ALIGN_CENTER, -115, 108);

    lv_obj_t *label55_4V = lv_label_create(screen_Home, NULL);
    lv_label_set_text(label55_4V, "55.4#94aeb4 V ");
    lv_label_set_recolor(label55_4V, "true");
    lv_obj_set_style_local_text_color(label55_4V, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(label55_4V, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(label55_4V, NULL, LV_ALIGN_CENTER, -110, -75);

    lv_obj_t *label54_0V = lv_label_create(screen_Home, NULL);
    lv_label_set_text(label54_0V, "54.0#94aeb4 V ");
    lv_label_set_recolor(label54_0V, "true");
    lv_obj_set_style_local_text_color(label54_0V, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(label54_0V, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(label54_0V, NULL, LV_ALIGN_CENTER, -135, -40);

    lv_obj_t *label50_7V = lv_label_create(screen_Home, NULL);
    lv_label_set_text(label50_7V, "50.7#94aeb4 V ");
    lv_label_set_recolor(label50_7V, "true");
    lv_obj_set_style_local_text_color(label50_7V, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(label50_7V, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(label50_7V, NULL, LV_ALIGN_CENTER, -135, 40);

    lv_obj_t *label49_3V = lv_label_create(screen_Home, NULL);
    lv_label_set_text(label49_3V, "49.3#94aeb4 V ");
    lv_label_set_recolor(label49_3V, "true");
    lv_obj_set_style_local_text_color(label49_3V, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(label49_3V, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(label49_3V, NULL, LV_ALIGN_CENTER, -110, 75);

    lv_obj_t *lotusImg = lv_img_create(screen_Home, NULL);
    lv_img_set_src(lotusImg, &ui_img_lotus);
    lv_obj_align(lotusImg, NULL, LV_ALIGN_CENTER, -136, -106);

    selectImg = lv_img_create(screen_Home, NULL);
    lv_img_set_src(selectImg, &ui_img_select);
    lv_obj_align(selectImg, NULL, LV_ALIGN_IN_BOTTOM_MID, 65, 0);

    lv_obj_t *iconEnergy = lv_label_create(screen_Home, NULL);
    lv_label_set_text(iconEnergy, "\uf0e7");
    lv_obj_set_style_local_text_color(iconEnergy, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFF8DE));
    lv_obj_set_style_local_text_font(iconEnergy, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_icon);
    lv_obj_align(iconEnergy, NULL, LV_ALIGN_CENTER, 65, 102);
    buttonEnergy = lv_btn_create(screen_Home, NULL);
    lv_obj_set_size(buttonEnergy, 25, 25);
    lv_obj_align(buttonEnergy, iconEnergy, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_bg_opa(buttonEnergy, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_opa(buttonEnergy, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_event_cb(buttonEnergy, ui_MenuClickEvent);

    lv_obj_t *iconBattery = lv_label_create(screen_Home, NULL);
    lv_label_set_text(iconBattery, "\uf376");
    lv_obj_set_style_local_text_color(iconBattery, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFF8DE));
    lv_obj_set_style_local_text_font(iconBattery, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_icon);
    lv_obj_align(iconBattery, NULL, LV_ALIGN_CENTER, 100, 102);
    buttonBattery = lv_btn_create(screen_Home, NULL);
    lv_obj_set_size(buttonBattery, 25, 25);
    lv_obj_align(buttonBattery, iconBattery, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_bg_opa(buttonBattery, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_opa(buttonBattery, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_event_cb(buttonBattery, ui_MenuClickEvent);

    lv_obj_t *iconDevice = lv_label_create(screen_Home, NULL);
    lv_label_set_text(iconDevice, "\uf2db");
    lv_obj_set_style_local_text_color(iconDevice, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFF8DE));
    lv_obj_set_style_local_text_font(iconDevice, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_icon);
    lv_obj_align(iconDevice, NULL, LV_ALIGN_CENTER, 135, 102);
    buttonDevice = lv_btn_create(screen_Home, NULL);
    lv_obj_set_size(buttonDevice, 25, 25);
    lv_obj_align(buttonDevice, iconDevice, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_bg_opa(buttonDevice, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_opa(buttonDevice, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_event_cb(buttonDevice, ui_MenuClickEvent);

    viewInformation = lv_obj_create(screen_Home, NULL);
    lv_obj_set_size(viewInformation, 140, 160);
    lv_obj_align(viewInformation, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 30);
    lv_obj_set_click(viewInformation, false);
    lv_obj_set_style_local_bg_opa(viewInformation, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_opa(viewInformation, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

    lv_obj_t *lineImg = lv_img_create(viewInformation, NULL);
    lv_img_set_src(lineImg, &ui_img_line);
    lv_obj_align(lineImg, screen_Home, LV_ALIGN_IN_RIGHT_MID, 0, -72);

    labelInformation = lv_label_create(viewInformation, NULL);
    lv_label_set_text(labelInformation, "Energy Information ");
    lv_obj_set_style_local_text_color(labelInformation, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(labelInformation, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_medium);
    lv_obj_align(labelInformation, screen_Home, LV_ALIGN_CENTER, 100, -72);
    lv_obj_set_auto_realign(labelInformation, true);

    labelInfo1 = lv_label_create(viewInformation, NULL);
    lv_label_set_text(labelInfo1, "Energy left ");
    lv_obj_set_style_local_text_color(labelInfo1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x94AEB4));
    lv_obj_set_style_local_text_font(labelInfo1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(labelInfo1, screen_Home, LV_ALIGN_IN_LEFT_MID, 245, -40);
    lv_obj_set_auto_realign(labelInfo1, true);

    valueInfo1 = lv_label_create(viewInformation, NULL);
    lv_label_set_text(valueInfo1, "3.520 ");
    lv_obj_set_style_local_text_color(valueInfo1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(valueInfo1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_big);
    lv_obj_align(valueInfo1, labelInfo1, LV_ALIGN_IN_LEFT_MID, 0, 15);
    lv_obj_set_auto_realign(valueInfo1, true);

    unitInfo1 = lv_label_create(viewInformation, NULL);
    lv_label_set_text(unitInfo1, "kWh ");
    lv_obj_set_style_local_text_color(unitInfo1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(unitInfo1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(unitInfo1, valueInfo1, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, -2);
    lv_obj_set_auto_realign(unitInfo1, true);

    labelInfo2 = lv_label_create(viewInformation, NULL);
    lv_label_set_text(labelInfo2, "Total charged ");
    lv_obj_set_style_local_text_color(labelInfo2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x94AEB4));
    lv_obj_set_style_local_text_font(labelInfo2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(labelInfo2, screen_Home, LV_ALIGN_IN_LEFT_MID, 245, -5);
    lv_obj_set_auto_realign(labelInfo2, true);

    valueInfo2 = lv_label_create(viewInformation, NULL);
    lv_label_set_text(valueInfo2, "3.0 ");
    lv_obj_set_style_local_text_color(valueInfo2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(valueInfo2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_big);
    lv_obj_align(valueInfo2, labelInfo2, LV_ALIGN_IN_LEFT_MID, 0, 15);
    lv_obj_set_auto_realign(valueInfo2, true);

    unitInfo2 = lv_label_create(viewInformation, NULL);
    lv_label_set_text(unitInfo2, "kWh ");
    lv_obj_set_style_local_text_color(unitInfo2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(unitInfo2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(unitInfo2, valueInfo2, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, -2);
    lv_obj_set_auto_realign(unitInfo2, true);

    labelInfo3 = lv_label_create(viewInformation, NULL);
    lv_label_set_text(labelInfo3, "Remaining time ");
    lv_obj_set_style_local_text_color(labelInfo3, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x94AEB4));
    lv_obj_set_style_local_text_font(labelInfo3, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(labelInfo3, screen_Home, LV_ALIGN_IN_LEFT_MID, 245, 30);
    lv_obj_set_auto_realign(labelInfo3, true);

    valueInfo3 = lv_label_create(viewInformation, NULL);
    lv_label_set_text(valueInfo3, "+2:30:25 ");
    lv_obj_set_style_local_text_color(valueInfo3, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(valueInfo3, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_big);
    lv_obj_align(valueInfo3, labelInfo3, LV_ALIGN_IN_LEFT_MID, 0, 15);
    lv_obj_set_auto_realign(valueInfo3, true);

    unitInfo3 = lv_label_create(viewInformation, NULL);
    lv_label_set_text(unitInfo3, " ");
    lv_obj_set_style_local_text_color(unitInfo3, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(unitInfo3, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(unitInfo3, valueInfo3, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, -2);
    lv_obj_set_auto_realign(unitInfo3, true);

    lv_obj_t *cornerImg = lv_img_create(screen_Home, NULL);
    lv_img_set_src(cornerImg, &ui_img_corner);
    lv_obj_align(cornerImg, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    lv_obj_t *iconCorner = lv_label_create(screen_Home, NULL);
    lv_label_set_text(iconCorner, "\uf013");
    lv_obj_set_style_local_text_color(iconCorner, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFF8DE));
    lv_obj_set_style_local_text_font(iconCorner, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_icon);
    lv_obj_align(iconCorner, cornerImg, LV_ALIGN_IN_RIGHT_MID, -8, -2);
    buttonCornerSetting = lv_btn_create(screen_Home, NULL);
    lv_obj_set_size(buttonCornerSetting, 25, 25);
    lv_obj_align(buttonCornerSetting, iconCorner, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_bg_opa(buttonCornerSetting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_opa(buttonCornerSetting, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_event_cb(buttonCornerSetting, ui_CornerClickEvent);

    labelTime = lv_label_create(screen_Home, NULL);
    lv_label_set_text(labelTime, "00:00 ");
    lv_obj_set_style_local_text_color(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_medium);
    lv_obj_align(labelTime, NULL, LV_ALIGN_IN_TOP_MID, 80, 0);

    labelTimeEx = lv_label_create(screen_Home, NULL);
    lv_label_set_text(labelTimeEx, "AM ");
    lv_obj_set_style_local_text_color(labelTimeEx, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(labelTimeEx, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(labelTimeEx, labelTime, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, -2);

    lv_obj_t *iconWiFiBack = lv_label_create(screen_Home, NULL);
    lv_label_set_text(iconWiFiBack, "\uf1eb");
    lv_obj_set_style_local_text_color(iconWiFiBack, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x94AEB4));
    lv_obj_set_style_local_text_font(iconWiFiBack, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_icon);
    lv_obj_align(iconWiFiBack, NULL, LV_ALIGN_IN_BOTTOM_MID, 45, -224);

    lv_obj_t *iconWiFiFront = lv_label_create(screen_Home, NULL);
    lv_label_set_text(iconWiFiFront, "\uf6ab");
    lv_obj_set_style_local_text_color(iconWiFiFront, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(iconWiFiFront, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_icon);
    lv_obj_align(iconWiFiFront, iconWiFiBack, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    lv_obj_t *iconLink = lv_label_create(screen_Home, NULL);
    lv_label_set_text(iconLink, "\uf0c1");
    lv_obj_set_style_local_text_color(iconLink, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(iconLink, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_icon);
    lv_obj_align(iconLink, NULL, LV_ALIGN_IN_BOTTOM_MID, 20, -224);
}

void ui_SettingInit()
{
    screen_Setting = lv_obj_create(NULL, NULL);
    ESP_LOGI(TAG, "Screen Setting init!");

    lv_obj_t *backgroundImg = lv_img_create(screen_Setting, NULL);
    lv_img_set_src(backgroundImg, &ui_img_cbg);
    lv_obj_align(backgroundImg, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *cornerImg = lv_img_create(screen_Setting, NULL);
    lv_img_set_src(cornerImg, &ui_img_corner);
    lv_obj_align(cornerImg, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

    lv_obj_t *iconCorner = lv_label_create(screen_Setting, NULL);
    lv_label_set_text(iconCorner, "\uf00d");
    lv_obj_set_style_local_text_color(iconCorner, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFF8DE));
    lv_obj_set_style_local_text_font(iconCorner, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_icon);
    lv_obj_align(iconCorner, cornerImg, LV_ALIGN_IN_RIGHT_MID, -10, -2);
    buttonCornerClose = lv_btn_create(screen_Setting, NULL);
    lv_obj_set_size(buttonCornerClose, 25, 25);
    lv_obj_align(buttonCornerClose, iconCorner, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_bg_opa(buttonCornerClose, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_opa(buttonCornerClose, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_event_cb(buttonCornerClose, ui_CornerClickEvent);

    lv_obj_t *lotusImg = lv_img_create(screen_Setting, NULL);
    lv_img_set_src(lotusImg, &ui_img_lotus);
    lv_obj_align(lotusImg, NULL, LV_ALIGN_CENTER, -136, -106);

    lv_obj_t *labelTime = lv_label_create(screen_Setting, NULL);
    lv_label_set_text(labelTime, "11:25 ");
    lv_obj_set_style_local_text_color(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(labelTime, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_medium);
    lv_obj_align(labelTime, NULL, LV_ALIGN_IN_TOP_MID, 80, 0);

    lv_obj_t *labelTimeEx = lv_label_create(screen_Setting, NULL);
    lv_label_set_text(labelTimeEx, "AM ");
    lv_obj_set_style_local_text_color(labelTimeEx, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(labelTimeEx, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    lv_obj_align(labelTimeEx, labelTime, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, -2);

    lv_obj_t *iconWiFiBack = lv_label_create(screen_Setting, NULL);
    lv_label_set_text(iconWiFiBack, "\uf1eb");
    lv_obj_set_style_local_text_color(iconWiFiBack, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x94AEB4));
    lv_obj_set_style_local_text_font(iconWiFiBack, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_icon);
    lv_obj_align(iconWiFiBack, NULL, LV_ALIGN_IN_BOTTOM_MID, 45, -224);

    lv_obj_t *iconWiFiFront = lv_label_create(screen_Setting, NULL);
    lv_label_set_text(iconWiFiFront, "\uf6ab");
    lv_obj_set_style_local_text_color(iconWiFiFront, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(iconWiFiFront, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_icon);
    lv_obj_align(iconWiFiFront, iconWiFiBack, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    lv_obj_t *iconLink = lv_label_create(screen_Setting, NULL);
    lv_label_set_text(iconLink, "\uf0c1");
    lv_obj_set_style_local_text_color(iconLink, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(iconLink, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &ui_font_icon);
    lv_obj_align(iconLink, NULL, LV_ALIGN_IN_BOTTOM_MID, 20, -224);

    rollerCutOffLevel = ui_RollerCreate(screen_Setting, ui_RollerEvent, 320 / 3, 95, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 35, LV_BORDER_SIDE_NONE, " Cut-off level ", " 0.500 kWh \n 1.000 kWh \n 1.500 kWh \n 2.000 kWh \n 2.500 kWh \n 3.000 kWh \n 3.500 kWh \n 4.000 kWh \n 4.500 kWh \n 5.000 kWh \n 5.500 kWh \n 6.000 kWh");
    rollerStartLevel = ui_RollerCreate(screen_Setting, ui_RollerEvent, 320 / 3, 95, NULL, LV_ALIGN_IN_TOP_MID, 0, 35, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT, " Start level ", " 0.500 kWh \n 1.000 kWh \n 1.500 kWh \n 2.000 kWh \n 2.500 kWh \n 3.000 kWh \n 3.500 kWh \n 4.000 kWh \n 4.500 kWh \n 5.000 kWh \n 5.500 kWh \n 6.000 kWh");
    rollerExternalVoltage = ui_RollerCreate(screen_Setting, ui_RollerEvent, 320 / 3, 95, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 35, LV_BORDER_SIDE_NONE, " External voltage ", " 50.0 V \n 50.1 V \n 50.2 V \n 50.3 V \n 50.4 V \n 50.5 V \n 50.6 V \n 50.7 V \n 50.8 V \n 50.9 V \n 51.0 V \n Disable ");
    rollerTotalEnergy = ui_RollerCreate(screen_Setting, ui_RollerEvent, 320 / 3, 95, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 40 + 95, LV_BORDER_SIDE_NONE, " Total energy ", " 2.500 kWh \n 5.000 kWh \n 7.500 kWh \n 10.000 kWh \n 12.500 kWh \n 15.000 kWh \n 17.500 kWh \n 20.000 kWh ");

    lv_obj_t *extInformation = lv_obj_create(screen_Setting, NULL);
    lv_obj_set_size(extInformation, 320 * 2 / 3, 95);
    lv_obj_align(extInformation, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 40 + 95);
    lv_obj_set_style_local_bg_opa(extInformation, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_color(extInformation, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x293031));
    lv_obj_set_style_local_border_color(extInformation, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_hex(0x293031));
    lv_obj_set_style_local_border_opa(extInformation, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_border_width(extInformation, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
    lv_obj_set_style_local_border_side(extInformation, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_LEFT);
    lv_obj_set_style_local_radius(extInformation, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
}

void ui_Init()
{
    ui_HomeInit();
    ui_SettingInit();
    lv_disp_load_scr(screen_Home);
}