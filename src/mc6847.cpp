#include "mc6847.h"
#include "osd.h"

#pragma GCC optimize ("Ofast")

constexpr int kOffsetX_ = 64;
constexpr int kOffsetY_ = 48;

#define CM_OLDREV   0
#define CM_NEW1     1
#define CM_NEW2     2
#define CM_GREEN    3

#define PAGE_SIZE 32*16

enum PaletteIndex {
  PAL_BLACK = 0,
  PAL_GREEN = 1,
  PAL_YELLOW = 2,
  PAL_BLUE = 3,
  PAL_RED = 4,
  PAL_BUFF = 5,
  PAL_CYAN = 6,
  PAL_ORANGE = 7,

  // Magenta replaced with orange to support ORANGE in text mode
  PAL_MAGENTA = 8,
  PAL_CYANBLUE = 9,
  PAL_LGREEN = 10,
  PAL_DGREEN = 11
};

MC6847::MC6847() : fabgl::VGA8Controller() {}

void MC6847::Init(byte *iomem) {
  begin();
  setResolution(VGA_640x480_60Hz);

  iomem_ = iomem;
  memset(iomem_, 0, 0x2000);

  page_buf_ = (byte *)malloc(PAGE_SIZE);

  refresh_func_ = &MC6847::RefreshTextSemiGraphic;

  InitColor(CM_NEW1);

  // Initialize semigraphic pattern
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 12; j++) {
      byte val = 0;
      if (j < 6) {
        if (i & 0x08) val |= 0xf0;
        if (i & 0x04) val |= 0x0f;
      } else {
        if (i & 0x02) val |= 0xf0;
        if (i & 0x01) val |= 0x0f;
      }
      semigr_font_0[i * 12 + j] = val;
    }
  }
  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 12; j++) {
      byte val = 0;
      if (j < 4) {
        if (i & 0x20) val |= 0xf0;
        if (i & 0x10) val |= 0x0f;
      } else if (j < 8) {
        if (i & 0x08) val |= 0xf0;
        if (i & 0x04) val |= 0x0f;
      } else {
        if (i & 0x02) val |= 0xf0;
        if (i & 0x01) val |= 0x0f;
      }
      semigr_font_1[i * 12 + j] = val;
    }
  }
}

void MC6847::SetFontFace(byte *font_base) {
  font_ = font_base;
}

void MC6847::SetCRTEffect(int enabled) {
  crt_effect_ = enabled;
  if (crt_effect_) {
    // Reset the redundant scanlines not to be painted in
    // vintage CRT mode.
    byte *font;
    int fgColor;
    int bgColor;
    int page_base = page_ * 0x200;
    int attr_base = page_base + 0x800;
    for (int sy = 48; sy < 480-48; sy += 2) {
      for (int sx = 64; sx < 640-64; sx += 2) {
        int x = (sx-64) >> 1;
        int y = (sy-48) >> 1;
        int yb = y / 12;
        int yf = y % 12;
        int offset = (yb<<5)+(x>>3);
        byte code = iomem_[page_base + offset];
        byte attr = iomem_[attr_base + offset];
        GetFontData(code, attr, &fgColor, &bgColor, &font);
        directSetPixel(sx, sy+1, bgColor);
        directSetPixel(sx+1, sy+1, bgColor);
      }
    }
  }
}

void MC6847::SetMode(int text, byte param) {
  int prev_mode = mode_;
  if (text) {
    page_ = (param & 0x30) >> 4;
    mode_ = 0;
    refresh_func_ = &MC6847::RefreshTextSemiGraphic;
  } else {
    switch (param & 0x8e) {
    case 0x80: // 128x96, not implemented
    case 0x0a: // color
    case 0x0c: // mono
      mode_ = 2;
      refresh_func_ = &MC6847::RefreshMidGraphic;
      break;
    case 0x8a: // color
    case 0x8c: // mono
      mode_ = 3;
      refresh_func_ = &MC6847::RefreshMidGraphic;
      break;
    case 0x0e:
      mode_ = 4;
      refresh_func_ = &MC6847::RefreshHiGraphic;
      break;
    case 0x8e:
      mode_ = 5;
      refresh_func_ = &MC6847::RefreshHiGraphic;
      break;
    default:
      mode_ = 5;
      refresh_func_ = &MC6847::RefreshHiGraphic;
      break;
    }
    if (mode_ != prev_mode) {
      if (mode_ == 5) {
          setPaletteItem(1, rgb_[PAL_LGREEN]);
      } else {
          setPaletteItem(1, rgb_[PAL_GREEN]);
      }
    }
  }
}

void MC6847::RefreshScreen() {
  (this->*refresh_func_)();
}

void MC6847::RefreshTextSemiGraphic() {
  byte *font;
  int fgColor;
  int bgColor;
  int page_base = page_ * 0x200;
  int attr_base = page_base + 0x800;

  for (int sy = 48; sy < 480-48; sy += 2) {
    for (int sx = 64; sx < 640-64; sx += 2) {
      int x = (sx-64) >> 1;
      int y = (sy-48) >> 1;
      int yb = y / 12;
      int yf = y % 12;
      int offset = (yb<<5)+(x>>3);
      byte code = iomem_[page_base + offset];
      byte attr = iomem_[attr_base + offset];
      GetFontData(code, attr, &fgColor, &bgColor, &font);
      byte xf = x & 0x7;
      byte fb = font[yf];
      int color = fb & (0x80 >> xf) ? fgColor : bgColor;
      directSetPixel(sx, sy, color);
      directSetPixel(sx+1, sy, color);
      if (!crt_effect_) {
        directSetPixel(sx, sy+1, color);
        directSetPixel(sx+1, sy+1, color);
      }
    }
  }
  if (osd_should_close_toast()) osd_show(0);
}

void MC6847::GetFontData(byte code, byte attr,
                         int *fgColor, int *bgColor, byte **fontData) {
  if (attr & 0x04) {
    // semigraphic
    *bgColor = semigr_tbl_[0];
    switch (attr & 0x08) {
    case 0:
      *fgColor = semigr_tbl_[((code & 0x70) >> 4) + 1];
      *fontData = &semigr_font_0[(code & 0x0f) * 12];
      break;
    case 0x08:
      *fgColor =
          semigr_tbl_[(((attr & 0x02) << 1) | ((code & 0xc0) >> 6)) + 1];
      *fontData = &semigr_font_1[(code & 0x3f) * 12];
      break;
    }
  } else {
    switch (attr & 0x03) {
    case 0x00:
      *fgColor = color_tbl_[0][1];
      *bgColor = color_tbl_[0][0];
      break;
    case 0x01:
      *fgColor = color_tbl_[0][0];
      *bgColor = color_tbl_[0][1];
      break;
    case 0x02:
      *fgColor = color_tbl_[0][3];
      *bgColor = color_tbl_[0][2];
      break;
    case 0x03:
      *fgColor = color_tbl_[0][2];
      *bgColor = color_tbl_[0][3];
      break;
    }
    if (code < 32 && !(attr & 0x08)) code = 32;
    if ((attr & 0x08) && (code < 96)) code += 128;

    if (96 <= code && code < 128)
      *fontData = iomem_ + 0x1600+(code-96)*16;
    else if (128 <= code && code < 224)
      *fontData = iomem_ + 0x1000+(code-128)*16;
    else
      *fontData = font_ + (code-32)*12;  // 8x12
  }
}

byte *MC6847::text_pos(int x, int y) {
  return iomem_ + page_*PAGE_SIZE + y*32 + x;
}

void MC6847::RefreshHiGraphic() {
  int fgColor = color_tbl_[mode_][1];
  int bgColor = color_tbl_[mode_][0];

  for (int sy = 48; sy < 480-48; sy += 2) {
    for (int sx = 64; sx < 640-64; sx += 2) {
      int x = (sx-64) >> 1;
      int y = (sy-48) >> 1;
      byte b = iomem_[(y<<5)+(x>>3)];
      byte i = x & 0x7;
      int color = b & (0x80 >> i) ? fgColor : bgColor;
      directSetPixel(sx, sy, color);
      directSetPixel(sx+1, sy, color);
      if (!crt_effect_) {
        directSetPixel(sx, sy+1, color);
        directSetPixel(sx+1, sy+1, color);
      }
    }
  }
}

void MC6847::RefreshMidGraphic() {
}

void MC6847::SavePage() {
  byte *page_addr = iomem_ + page_ * 0x200;
  memcpy(page_buf_, page_addr, PAGE_SIZE);
  memcpy(page_buf_ + PAGE_SIZE, page_addr + 0x800, PAGE_SIZE);
}

void MC6847::RestorePage() {
  byte *page_addr = iomem_ + page_ * 0x200;
  memcpy(page_addr, page_buf_, PAGE_SIZE);
  memcpy(page_addr + 0x800, page_buf_ + PAGE_SIZE, PAGE_SIZE);
}

void MC6847::InitColor(int colorMode) {
  if (colorMode == CM_GREEN) {
    rgb_[PAL_BLACK] = RGB888(0, 0, 0);
    rgb_[PAL_GREEN] = RGB888(0, 255, 0);
    rgb_[PAL_YELLOW] = RGB888(32, 255, 32);
    rgb_[PAL_BLUE] = RGB888(0, 32, 0);
    rgb_[PAL_RED] = RGB888(0, 100, 0);
    rgb_[PAL_BUFF] = RGB888(0, 24, 0);
    rgb_[PAL_CYAN] = RGB888(0, 224, 0);
    rgb_[PAL_MAGENTA] = RGB888(0, 128, 0);
    rgb_[PAL_ORANGE] = RGB888(0, 224, 0);
    rgb_[PAL_CYANBLUE] = RGB888(0, 200, 0); // CYAN/BLUE?
    rgb_[PAL_LGREEN] = RGB888(32, 255, 32);
    rgb_[PAL_DGREEN] = RGB888(0, 192, 0); // for screen 2
  } else {
    rgb_[PAL_BLACK] = RGB888(0, 0, 0);
    rgb_[PAL_GREEN] = RGB888(0, 255, 0);
    rgb_[PAL_YELLOW] = RGB888(255, 255, 192);
    rgb_[PAL_BLUE] = RGB888(0, 0, 255);
    rgb_[PAL_RED] = RGB888(255, 0, 0);
    rgb_[PAL_BUFF] = RGB888(96, 0, 0);
    rgb_[PAL_CYAN] = RGB888(0, 255, 255);
    rgb_[PAL_MAGENTA] = RGB888(255, 0, 255);
    rgb_[PAL_ORANGE] = RGB888(255, 128, 0);
    rgb_[PAL_CYANBLUE] = RGB888(0, 128, 255); // CYAN/BLUE?
    rgb_[PAL_LGREEN] = RGB888(64, 255, 64);
    rgb_[PAL_DGREEN] = RGB888(0, 192, 0);
  }
  setPaletteItem(0, rgb_[PAL_BLACK]);
  setPaletteItem(1, rgb_[PAL_GREEN]);
  setPaletteItem(2, rgb_[PAL_YELLOW]);
  setPaletteItem(3, rgb_[PAL_BLUE]);
  setPaletteItem(4, rgb_[PAL_RED]);
  setPaletteItem(5, rgb_[PAL_BUFF]);
  setPaletteItem(6, rgb_[PAL_CYAN]);
  setPaletteItem(7, rgb_[PAL_ORANGE]);

  // Semi Graphic
  semigr_tbl_[0] = PAL_BLACK;
  semigr_tbl_[1] = PAL_GREEN;
  semigr_tbl_[2] = PAL_YELLOW;
  semigr_tbl_[3] = PAL_BLUE;
  semigr_tbl_[4] = PAL_RED;
  semigr_tbl_[5] = PAL_BUFF;
  semigr_tbl_[6] = PAL_CYAN;
  semigr_tbl_[7] = PAL_MAGENTA;
  semigr_tbl_[8] = PAL_ORANGE;

  // Text Mode
  color_tbl_[0][0] = PAL_BLACK;
  color_tbl_[0][1] = PAL_GREEN;
  color_tbl_[0][2] = PAL_BUFF;
  color_tbl_[0][3] = PAL_ORANGE;

  // Screen 2
  color_tbl_[2][0] = PAL_GREEN;
  color_tbl_[2][1] = PAL_YELLOW;
  color_tbl_[2][2] = PAL_BLUE;
  color_tbl_[2][3] = PAL_RED;

  // Screen 3
  color_tbl_[3][0] = PAL_BLUE;
  color_tbl_[3][1] = PAL_CYANBLUE;
  color_tbl_[3][2] = PAL_MAGENTA;
  color_tbl_[3][3] = PAL_ORANGE;

  // Screen 4
  color_tbl_[4][0] = PAL_BLACK;
  color_tbl_[4][1] = PAL_GREEN;

  // Screen 5
  color_tbl_[5][0] = PAL_BLACK;
  color_tbl_[5][1] = PAL_GREEN; // Set to LGREEN at mode switch

  switch (colorMode) {
  case CM_OLDREV:
    color_tbl_[0][2] = PAL_BLACK;
    color_tbl_[0][3] = PAL_CYAN;

    color_tbl_[5][1] = PAL_CYAN;
    break;
  case CM_NEW1:
    color_tbl_[0][2] = PAL_BLACK;
    color_tbl_[0][3] = PAL_ORANGE;
    break;
  case CM_NEW2:
    break;
  case CM_GREEN:
    setPaletteItem(0, rgb_[PAL_DGREEN]);
    break;
  }
}
