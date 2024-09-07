#include "fenster.h"
#include<stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "renderer.h"
#include "atlas.inl"

#define BUFFER_SIZE 16384

typedef uint8_t byte;

static mu_Rect tex_buf[BUFFER_SIZE];
static mu_Rect src_buf[BUFFER_SIZE];
static mu_Color color_buf[BUFFER_SIZE];

static int buf_idx;

static struct fenster window = {.title="A window", .width=800, .height=600};

static mu_Rect clip_rect;

int mousex = 0, mousey = 0, mouseclick = 0;

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

static inline bool same_size(const mu_Rect* a, const mu_Rect* b) {
  return a->w == b->w && a->h == b->h;
}

static inline byte texture_color(const mu_Rect* tex, int x, int y) {
  assert(x < tex->w);
  x = x + tex->x;
  assert(y < tex->h);
  y = y + tex->y;
  return atlas_texture[y*ATLAS_WIDTH + x];
}

static inline int color(byte r, byte g, byte b) {  // ignore alpha channel for now
  int result = r;
  result = result << 8;
  result = result | g;
  result = result << 8;
  result = result | b;
  return result;
}

static inline int greyscale(byte c) {
  return color(c, c, c);
}

int min(int a, int b) {
  return a < b ? a : b;
}

int max(int a, int b) {
  return a > b ? a : b;
}

static void flush(void) {
  if (buf_idx == 0) { return; }

  // draw things based on texture, vertex, color
  for (int i = 0; i < buf_idx; i++) {
    mu_Rect* src = &src_buf[i];
    mu_Rect* tex = &tex_buf[i];
    int c = color(color_buf[i].r, color_buf[i].g, color_buf[i].b);
    // draw
    int ystart = max(src->y, clip_rect.y);
    int yend = min(src->y+src->h, clip_rect.y+clip_rect.h);
    int xstart = max(src->x, clip_rect.x);
    int xend = min(src->x+src->w, clip_rect.x+clip_rect.w);
    for (int y = ystart; y < yend; y++) {
      for (int x = xstart; x < xend; x++) {
        assert(within_rect(*src, x, y));
        assert(within_rect(clip_rect, x, y));
        // hacky but sufficient for us
        if (same_size(src, tex)) {
          // read color from texture
          byte tc = texture_color(tex, x-src->x, y-src->y);
          fenster_pixel(&window, x, y) |= greyscale(tc);
        }
        else {
          // use color from operation
          fenster_pixel(&window, x, y) = c;
        }
      }
    }
  }

  buf_idx = 0;
}


static void push_quad(mu_Rect src, mu_Rect tex, mu_Color color) {
  if (buf_idx == BUFFER_SIZE) { flush(); }

  memcpy(&tex_buf[buf_idx], &tex, sizeof(mu_Rect));
  memcpy(&src_buf[buf_idx], &src, sizeof(mu_Rect));
  memcpy(&color_buf[buf_idx], &color, sizeof(mu_Color));

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
  clip_rect = rect;
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


int r_mouse_down(void) {
  if (window.mouse && !mouseclick) {
    mouseclick = 1;
    return 1;
  }
  return 0;
}

int r_mouse_up(void) {
  if (!window.mouse && mouseclick) {
    mouseclick = 0;
    return 1;
  }
  return 0;
}

int r_mouse_moved(int *new_mousex, int *new_mousey) {
  if (window.x != mousex || window.y != mousey) {
    *new_mousex = window.x;
    *new_mousey = window.y;
    return 1;
  }
  return 0;
}
