#ifndef RENDERER_H
#define RENDERER_H

#include "microui.h"
#include <stdint.h>

void r_init(void);
void r_draw_rect(mu_Rect rect, mu_Color color);
void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color);
void r_draw_icon(int id, mu_Rect rect, mu_Color color);
 int r_get_text_width(const char *text, int len);
 int r_get_text_height(void);
void r_set_clip_rect(mu_Rect rect);
void r_clear(mu_Color color);
void r_present(void);
// Can only be checked once per frame; side-effecting.
 int r_mouse_down(void);
 int r_mouse_up(void);
// Can only be checked once per frame; side-effecting.
 int r_mouse_moved(int *x, int *y);
// Can only be checked once per key per frame; side-effecting.
 int r_key_down(int key);
 int r_key_up(int key);
 int r_ctrl_pressed(void);
 int r_shift_pressed(void);
 int r_alt_pressed(void);
 int64_t r_get_time(void);
 void r_sleep(int64_t ms);

#endif

