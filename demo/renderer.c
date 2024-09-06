#include "fenster.h"
#include<stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "renderer.h"
#include "atlas.inl"

#define BUFFER_SIZE 16384

typedef uint8_t byte;

static mu_Rect tex_buf[BUFFER_SIZE];
static mu_Rect vert_buf[BUFFER_SIZE];
static byte      color_buf[BUFFER_SIZE * 4];

static int buf_idx;

static struct fenster window = (struct fenster){.title="A window", .width=800, .height=600};

static mu_Rect clip_rect;

void *r_window(void) {
  return &window;
}

void r_init(void) {
  /* init SDL window */
  window.buf = malloc(window.width * window.height * sizeof(*window.buf));
  r_clear(mu_color(0, 0, 0, 255));
  fenster_open(&window);

  /* init texture */
}

static inline bool within(int c, int lo, int hi) {
  return c >= lo && c < hi;
}

static inline bool within_rect(mu_Rect rect, int x, int y) {
  return within(x, rect.x, rect.x+rect.w)
        && within(y, rect.y, rect.y+rect.h);
}


static void flush(void) {
  if (buf_idx == 0) { return; }

  // HERE: draw things based on texture, vertex, color
  for (int i = 0; i < BUFFER_SIZE; i++) {
    mu_Rect* vert = &vert_buf[i];
    mu_Rect* tex = &tex_buf[i];
    int color = color_buf[i];  // red
    color = color << 8;
    color = color & color_buf[i+1];  // green
    color = color << 8;
    color = color & color_buf[i+2];  // blue
    // ignore alpha channel
    // draw
    for (int y = vert->y; y < vert->y+vert->h; y++) {
      for (int x = vert->x; x < vert->x+vert->w; x++) {
        if (within_rect(clip_rect, x, y)) {
          fenster_pixel(&window, x, y) = color;
        }
      }
    }
  }

  buf_idx = 0;
}


static void push_quad(mu_Rect vert, mu_Rect tex, mu_Color color) {
  if (buf_idx == BUFFER_SIZE) { flush(); }

  int color_idx = buf_idx *  4;

  memcpy(&tex_buf[buf_idx], &tex, sizeof(mu_Rect));
  memcpy(&vert_buf[buf_idx], &vert, sizeof(mu_Rect));
  memcpy(color_buf + color_idx, &color, 4);

  buf_idx++;
}


void r_draw_rect(mu_Rect rect, mu_Color color) {
  push_quad(rect, atlas[ATLAS_WHITE], color);
}


void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color) {
  mu_Rect dst = { pos.x, pos.y, 0, 0 };
  for (const char *p = text; *p; p++) {
    if ((*p & 0xc0) == 0x80) { continue; }
    int chr = mu_min((unsigned char) *p, 127);
    mu_Rect src = atlas[ATLAS_FONT + chr];
    dst.w = src.w;
    dst.h = src.h;
    push_quad(dst, src, color);
    dst.x += dst.w;
  }
}


void r_draw_icon(int id, mu_Rect rect, mu_Color color) {
  mu_Rect src = atlas[id];
  int x = rect.x + (rect.w - src.w) / 2;
  int y = rect.y + (rect.h - src.h) / 2;
  push_quad(mu_rect(x, y, src.w, src.h), src, color);
}


int r_get_text_width(const char *text, int len) {
  int res = 0;
  for (const char *p = text; *p && len--; p++) {
    if ((*p & 0xc0) == 0x80) { continue; }
    int chr = mu_min((unsigned char) *p, 127);
    res += atlas[ATLAS_FONT + chr].w;
  }
  return res;
}


int r_get_text_height(void) {
  return 18;
}


void r_set_clip_rect(mu_Rect rect) {
  flush();
  memcpy(&clip_rect, &rect, sizeof(mu_Rect));
}

uint32_t r_color(mu_Color clr) {
  return ((uint32_t)clr.a << 24) | ((uint32_t)clr.b << 16) | ((uint32_t)clr.g << 8) | clr.r;
}

void r_clear(mu_Color clr) {
  flush();
  for (int i = 0; i < window.width * window.height; i++) {
    window.buf[i] = r_color(clr);
  }
}


void r_present(void) {
  flush();
  fenster_loop(&window);
}
