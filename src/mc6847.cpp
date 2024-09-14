
#include "mc6847.h"
#include "fabgl.h"

//extern uint8_t mc6847_font[];
extern uint8_t mem[];

constexpr int kOffsetX_ = 64;
constexpr int kOffsetY_ = 48;

#pragma GCC optimize ("Ofast")

MC6847::MC6847() : fabgl::VGA8Controller() {
}

void MC6847::Init(uint8_t *iomem) {
  begin();
  setResolution(VGA_640x480_60Hz);

  iomem_ = iomem;
  memset(iomem_, 0, 0x2000);
  font_internal_ = &mem[0x524a]; //mc6847_font;

  setPaletteItem(0, RGB888(0, 0, 0));
  setPaletteItem(1, RGB888(255, 0, 0));
  setPaletteItem(2, RGB888(0, 255, 0));
  setPaletteItem(3, RGB888(0, 0, 255));
  setPaletteItem(4, RGB888(255, 255, 0));
  setPaletteItem(5, RGB888(255, 0, 255));
  setPaletteItem(6, RGB888(0, 255, 255));
  setPaletteItem(7, RGB888(255, 255, 255));
}

void MC6847::RefreshScreen() {
  unsigned char *font;
  int fgColor = 2;
  int bgColor = 0;
  int page_base;
  int attr_base;

  page_base = current_page_ * 0x200;
  attr_base = page_base + 0x800;

  for (int sy = 48; sy < 480-48; sy += 2) {
    for (int sx = 64; sx < 640-64; sx += 2) {
      int x = (sx-64) >> 1;
      int y = (sy-48) >> 1;
      int yb = y / 12;
      int yf = y % 12;
      int xb = x >> 3;
      uint8_t code = iomem_[page_base + yb*32+xb];
      uint8_t attr = iomem_[attr_base + yb*32+xb];

      if (code < 32 && !(attr & 0x08)) code = 32;
      if ((attr & 0x08) && (code < 96)) code += 128;

      if (96 <= code && code < 128)
        font = iomem_ + 0x1600+(code-96)*16;
      else if (128 <= code && code < 224)
        font = iomem_ + 0x1000+(code-128)*16;
      else
        font = font_internal_ + (code-32)*12;  // 8x12
      uint8_t xf = x & 0x7;
      uint8_t fb = font[yf];
      uint8_t inv = attr & 0x01;
      uint8_t pix = (fb & (0x80 >> xf)) ? 1 : 0;
      int color = pix ^ inv ? fgColor : bgColor;
      directSetPixel(sx, sy, color);
      directSetPixel(sx+1, sy, color);
    }
  }
}
