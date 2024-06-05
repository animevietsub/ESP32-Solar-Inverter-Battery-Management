#include <ui_helpers.h>

LV_FONT_DECLARE(ui_font_percent);
LV_FONT_DECLARE(ui_font_medium);
LV_FONT_DECLARE(ui_font_big);
LV_FONT_DECLARE(ui_font_small);
LV_FONT_DECLARE(ui_font_icon);

void ui_AnimSetOpa(lv_obj_t *obj, int32_t value)
{
    lv_obj_set_style_local_image_opa(obj, NULL, LV_STATE_DEFAULT, value);
    lv_obj_set_style_local_text_opa(obj, NULL, LV_STATE_DEFAULT, value);
}

void ui_AnimSetXMove(lv_obj_t *obj, int32_t value)
{
    lv_obj_set_x(obj, value);
}

void ui_IconShowAnimation(lv_obj_t *obj, int delay)
{
    lv_anim_t animOpa;
    lv_anim_init(&animOpa);
    animOpa.time = 500;
    animOpa.var = obj;
    animOpa.exec_cb = (lv_anim_exec_xcb_t)ui_AnimSetOpa;
    animOpa.start = 0;
    animOpa.end = 255;
    lv_anim_path_t path;
    lv_anim_path_init(&path);
    lv_anim_path_set_cb(&path, lv_anim_path_ease_out);
    lv_anim_set_path(&animOpa, &path);
    animOpa.playback_time = 0;
    animOpa.playback_delay = 0;
    animOpa.repeat_cnt = 0;
    animOpa.repeat_delay = 0;
    animOpa.early_apply = 1;
    lv_anim_start(&animOpa);
}

void ui_InfoShowAnimation(lv_obj_t *obj, int delay)
{
    lv_anim_t animOpa;
    lv_anim_init(&animOpa);
    animOpa.time = 500;
    animOpa.var = obj;
    animOpa.exec_cb = (lv_anim_exec_xcb_t)ui_AnimSetOpa;
    animOpa.start = 0;
    animOpa.end = 255;
    lv_anim_path_t path1;
    lv_anim_path_init(&path1);
    lv_anim_set_path(&animOpa, &path1);
    lv_anim_path_set_cb(&path1, lv_anim_path_ease_out);
    animOpa.playback_time = 0;
    animOpa.playback_delay = 0;
    animOpa.repeat_cnt = 0;
    animOpa.repeat_delay = 0;
    animOpa.early_apply = 1;
    lv_anim_start(&animOpa);

    lv_anim_t animXMove;
    lv_anim_init(&animXMove);
    animXMove.time = 600;
    animXMove.var = obj;
    animXMove.exec_cb = (lv_anim_exec_xcb_t)ui_AnimSetXMove;
    lv_obj_realign(obj);
    animXMove.start = lv_obj_get_x(obj) + 100;
    animXMove.end = lv_obj_get_x(obj);
    lv_anim_path_t path2;
    lv_anim_path_init(&path2);
    lv_anim_set_path(&animXMove, &path2);
    lv_anim_path_set_cb(&path2, lv_anim_path_ease_out);
    animXMove.playback_time = 0;
    animXMove.playback_delay = 0;
    animXMove.repeat_cnt = 0;
    animXMove.repeat_delay = 0;
    animXMove.early_apply = 0;
    lv_anim_start(&animXMove);
}

lv_obj_t *ui_RollerCreate(lv_obj_t *parent, lv_coord_t w, lv_coord_t h, lv_obj_t *base, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs, lv_border_side_t border_side, const char *label_text, const char *options_text)
{
    lv_obj_t *ui_Roller;
    ui_Roller = lv_obj_create(parent, NULL);
    lv_obj_set_size(ui_Roller, w, h);
    lv_obj_align(ui_Roller, base, align, x_ofs, y_ofs);
    lv_obj_set_style_local_bg_opa(ui_Roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_color(ui_Roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x293031));
    lv_obj_set_style_local_border_color(ui_Roller, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_hex(0x293031));
    lv_obj_set_style_local_border_opa(ui_Roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_border_width(ui_Roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
    lv_obj_set_style_local_border_side(ui_Roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, border_side);
    lv_obj_set_style_local_radius(ui_Roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_pad_left(ui_Roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_pad_right(ui_Roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_pad_top(ui_Roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
    lv_obj_set_style_local_pad_bottom(ui_Roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);

    lv_obj_t *label = lv_label_create(ui_Roller, NULL);
    lv_label_set_text(label, label_text);
    lv_obj_set_style_local_text_color(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &ui_font_big);
    lv_obj_align(label, ui_Roller, LV_ALIGN_IN_TOP_MID, 0, 0);

    // lv_obj_t *ext_label = lv_label_create(ui_Roller, NULL);
    // lv_label_set_text(ext_label, ext_label_text);
    // lv_obj_set_style_local_text_color(ext_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x94AEB4));
    // lv_obj_set_style_local_text_font(ext_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &ui_font_small);
    // lv_obj_align(ext_label, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_t *roller = lv_roller_create(ui_Roller, NULL);
    lv_roller_set_options(roller, options_text, LV_ROLLER_MODE_INIFINITE);
    lv_obj_set_width(roller, 80);
    lv_obj_set_height(roller, 60);
    lv_obj_set_style_local_text_color(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x839DA4));
    lv_roller_set_align(roller, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_style_local_text_font(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &ui_font_medium);
    lv_obj_set_style_local_text_line_space(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
    lv_obj_set_style_local_radius(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 5);
    lv_obj_set_style_local_bg_color(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x252525));
    lv_obj_set_style_local_bg_opa(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_bg_grad_color(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x080C10));
    lv_obj_set_style_local_bg_grad_dir(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_outline_color(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x7BAAC5));
    lv_obj_set_style_local_outline_opa(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_obj_set_style_local_outline_width(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
    lv_obj_set_style_local_outline_pad(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_border_opa(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

    lv_obj_set_style_local_text_color(roller, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_obj_set_style_local_text_font(roller, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, &ui_font_medium);
    lv_obj_set_style_local_bg_color(roller, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_color_hex(0x3179CD));
    lv_obj_set_style_local_bg_opa(roller, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, LV_OPA_COVER);

    lv_obj_align(roller, label, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);

    return ui_Roller;
}