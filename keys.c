#include "fenster.h"
int main() {
  struct fenster window = {.title = "A window", .width = 800, .height = 600};
  window.buf = malloc(window.width * window.height * sizeof(*window.buf));
  memset(window.buf, 0, window.width * window.height * sizeof(*window.buf));

  fenster_open(&window);

  for (;;) {
    int key_pressed = 0;
    if (window.keys[0x1b]) {
      break;
    }
    for (int i = 0; i < 256; i++) {
      if (window.keys[i]) {
        printf("Key '%c' is pressed\n", i);
        key_pressed = 1;
      }
      fenster_loop(&window);
    }
    // if (key_pressed) {
    //   if (window.mod & 1) {
    //     printf("ctrl\n");
    //   }
    //   if (window.mod & 2) {
    //     printf("shift\n");
    //   }
    //   if (window.mod & 4) {
    //     printf("alt\n");
    //   }
    // }
  }

  fenster_close(&window);
}
