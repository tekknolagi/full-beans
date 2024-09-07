#ifndef RENDERER_H
#define RENDERER_H

#include "microui.h"

void r_init(void);
void *r_window(void);
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
// Can only be checked once per frame; side-effecting.
 int r_mouse_up(void);
// Can only be checked once per frame; side-effecting.
 int r_mouse_moved(int *x, int *y);

#endif

