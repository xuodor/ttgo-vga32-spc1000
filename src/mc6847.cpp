
#include "mc6847.h"

extern uint8_t mc6847_font[];

#pragma GCC optimize ("O2")

MC6847::MC6847() : fabgl::VGA8Controller() {

}

void MC6847::InitVRAM(uint8_t *mem) {
  vram_ = mem;
  memset(vram_, 0, 0x2000);
  font_internal_ = mc6847_font;
}

void MC6847::RefreshScreen() {
  unsigned char *font;
  int fgColor = 2;
  int bgColor = 0;
  int page_base;
  int attr_base;

  page_base = current_page_ * 0x200;
  attr_base = page_base + 0x800;

  for (int sy = 0; sy < 480; ++sy) {
    for (int sx = 0; sx < 640; sx++) {
      if (64 <= sx && sx < 640-64 && 48 <= sy && sy < 480-48) {
        int x = (sx-64) >> 1;
        int y = (sy-48) >> 1;
        int yb = y / 12;
        int yf = y % 12;
        int xb = x >> 3;
        uint8_t code = vram_[page_base + yb*32+xb];
        uint8_t attr = vram_[attr_base + yb*32+xb];

        font = font_internal_ + (code - 32) * 12;  // 8x12
        uint8_t xf = x % 8;
        uint8_t fb = font[yf];
        uint8_t inv = attr & 0x01;
        uint8_t pix = (fb & (0x80 >> xf)) ? 1 : 0;
        int color = pix ^ inv ? fgColor : bgColor;
        directSetPixel(sx, sy, color);
      }
    }
  }
}

void MC6847::PutChar(int x, int y, int ascii, int attr) {
  unsigned char *font;
  int fgColor = 2;
  int bgColor = 0;

  font = font_internal_ + (ascii - 32) * 12;  // 8x12
  uint8_t inv = attr & 0x01;

  for (int r = 0; r < 12; r++) {
    for (int c = 0; c < 8; c++) {
      int color = ((font[r] & (0x80 >> c)) ^ inv) ? fgColor : bgColor;
      DrawCharDot(c + x, y + r, color);
    }
  }
}

void MC6847::DrawCharDot(int x, int y, int c) {
  directSetPixel(kOffsetX_+x*2,   kOffsetY_+y*2, c);
  directSetPixel(kOffsetX_+x*2+1, kOffsetY_+y*2, c);
}
