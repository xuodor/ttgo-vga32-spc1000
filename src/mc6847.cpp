
#include "mc6847.h"

extern uint8_t mc6847_font[];

#pragma GCC optimize ("O2")

MC6847::MC6847() : fabgl::VGA8Controller() {

}

void MC6847::InitVRAM(uint8_t *mem) {
  vram_ = mem;
  font_internal_ = mc6847_font;
}

void MC6847::RefreshScreen() {
  int page;
  int attr;

  page = current_page_ * 0x200;
  attr = page + 0x800;

  for (int y = 0; y < 16; y++) {
    for (int x = 0; x < 32; x++) {
      PutChar(x*8, y*12, vram_[page + y*32+x], vram_[attr + y*32+x]);
    }
  }
}

void MC6847::PutChar(int x, int y, int ascii, int attr) {
  unsigned char *font;
  int fgColor = 2;
  int bgColor = 0;

  font = font_internal_ + (ascii - 32) * 12;  // 8x12

  for (int r = 0; r < 12; r++) {
    for (int c = 0; c < 8; c++) {
      int color = font[r] & (0x80 >> c) ? fgColor : bgColor;
      DrawCharDot(c + x, y + r, color);
    }
  }
}

void MC6847::DrawCharDot(int x, int y, int c) {
  directSetPixel(kOffsetX_+x*2,   kOffsetY_+y*2, c);
  directSetPixel(kOffsetX_+x*2+1, kOffsetY_+y*2, c);
}
