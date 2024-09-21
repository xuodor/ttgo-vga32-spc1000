
#include "mc6847.h"

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

  page_buf_ = (uint8_t *)malloc(PAGE_SIZE);
  refresh_func_ = &MC6847::RefreshTextSemiGraphic;

  InitColor(CM_NEW1);

  // Initialize semigraphic pattern
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 12; j++) {
      unsigned char val = 0;
      if (j < 6) {
        if (i & 0x08)
          val |= 0xf0;
        if (i & 0x04)
          val |= 0x0f;
      } else {
        if (i & 0x02)
          val |= 0xf0;
        if (i & 0x01)
          val |= 0x0f;
      }
      semiGrFont0[i * 12 + j] = val;
    }
  }
  for (int i = 0; i < 64; i++) {
    for (int j = 0; j < 12; j++) {
      unsigned char val = 0;
      if (j < 4) {
        if (i & 0x20)
          val |= 0xf0;
        if (i & 0x10)
          val |= 0x0f;
      } else if (j < 8) {
        if (i & 0x08)
          val |= 0xf0;
        if (i & 0x04)
          val |= 0x0f;
      } else {
        if (i & 0x02)
          val |= 0xf0;
        if (i & 0x01)
          val |= 0x0f;
      }
      semiGrFont1[i * 12 + j] = val;
    }
  }
}

void MC6847::SetMode(int command, byte param) {
  printf("mode cmd:%d param:%02x\n", command, param);
  int prev_mode = mode_;
  switch (command) {
  case GM_TEXT:
    page_ = (param & 0x30) >> 4;
    mode_ = 0;
    refresh_func_ = &MC6847::RefreshTextSemiGraphic;
    //UpdateRect
    break;
//  case GM_TEXT_PAGE:
//    page_ = param;
    // UpdateTExt
//    break;
  case GM_GRAPHIC:
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
      // UpdateGr
    }
  }
  Serial.printf("mode:%d\n", mode_);
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
      uint8_t code = iomem_[page_base + offset];
      uint8_t attr = iomem_[attr_base + offset];
      GetFontData(code, attr, &fgColor, &bgColor, &font);
      uint8_t xf = x & 0x7;
      uint8_t fb = font[yf];
      int color = fb & (0x80 >> xf) ? fgColor : bgColor;
      directSetPixel(sx, sy, color);
      directSetPixel(sx+1, sy, color);
    }
  }
}

void MC6847::GetFontData(byte code, byte attr,
                         int *fgColor, int *bgColor, byte **fontData) {
  if (attr & 0x04) {
    // semigraphic
    *bgColor = semiColorTbl[0];
    switch (attr & 0x08) {
    case 0:
      *fgColor = semiColorTbl[((code & 0x70) >> 4) + 1];
      *fontData = &semiGrFont0[(code & 0x0f) * 12];
      break;
    case 0x08:
      *fgColor =
          semiColorTbl[(((attr & 0x02) << 1) | ((code & 0xc0) >> 6)) + 1];
      *fontData = &semiGrFont1[(code & 0x3f) * 12];
      break;
    }
  } else {
    switch (attr & 0x03) {
    case 0x00:
      *fgColor = colorTbl[0][1];
      *bgColor = colorTbl[0][0];
      break;
    case 0x01:
      *fgColor = colorTbl[0][0];
      *bgColor = colorTbl[0][1];
      break;
    case 0x02:
      *fgColor = colorTbl[0][3];
      *bgColor = colorTbl[0][2];
      break;
    case 0x03:
      *fgColor = colorTbl[0][2];
      *bgColor = colorTbl[0][3];
      break;
    }
    if (code < 32 && !(attr & 0x08)) code = 32;
    if ((attr & 0x08) && (code < 96)) code += 128;

    if (96 <= code && code < 128)
      *fontData = iomem_ + 0x1600+(code-96)*16;
    else if (128 <= code && code < 224)
      *fontData = iomem_ + 0x1000+(code-128)*16;
    else
      *fontData = font_internal_ + (code-32)*12;  // 8x12
  }
}

byte *MC6847::text_pos(int x, int y) {
  return iomem_ + page_*PAGE_SIZE + y*32 + x;
}

void MC6847::RefreshHiGraphic() {
  int fgColor = colorTbl[mode_][1];
  int bgColor = colorTbl[mode_][0];

  for (int sy = 48; sy < 480-48; sy += 2) {
    for (int sx = 64; sx < 640-64; sx += 2) {
      int x = (sx-64) >> 1;
      int y = (sy-48) >> 1;
      uint8_t b = iomem_[(y<<5)+(x>>3)];
      byte i = x & 0x7;
      int color = b & (0x80 >> i) ? fgColor : bgColor;
      directSetPixel(sx, sy, color);
      directSetPixel(sx+1, sy, color);
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
    colorMap[COLOR_BLACK] = RGB888(0, 0, 0);
    colorMap[COLOR_GREEN] = RGB888(0, 255, 0);
    colorMap[COLOR_YELLOW] = RGB888(32, 255, 32);
    colorMap[COLOR_BLUE] = RGB888(0, 32, 0);
    colorMap[COLOR_RED] = RGB888(0, 100, 0);
    colorMap[COLOR_BUFF] = RGB888(0, 24, 0);
    colorMap[COLOR_CYAN] = RGB888(0, 224, 0);
    colorMap[COLOR_MAGENTA] = RGB888(0, 128, 0);
    colorMap[COLOR_ORANGE] = RGB888(0, 224, 0);
    colorMap[COLOR_CYANBLUE] = RGB888(0, 200, 0); // CYAN/BLUE?
    colorMap[COLOR_LGREEN] = RGB888(32, 255, 32);
    colorMap[COLOR_DGREEN] = RGB888(0, 192, 0); // for screen 2
  } else {
    colorMap[COLOR_BLACK] = RGB888(0, 0, 0);
    colorMap[COLOR_GREEN] = RGB888(0, 255, 0);
    colorMap[COLOR_YELLOW] = RGB888(255, 255, 192);
    colorMap[COLOR_BLUE] = RGB888(0, 0, 255);
    colorMap[COLOR_RED] = RGB888(255, 0, 0);
    colorMap[COLOR_BUFF] = RGB888(96, 0, 0);
    colorMap[COLOR_CYAN] = RGB888(0, 255, 255);
    colorMap[COLOR_MAGENTA] = RGB888(255, 0, 255);
    colorMap[COLOR_ORANGE] = RGB888(255, 128, 0);
    colorMap[COLOR_CYANBLUE] = RGB888(0, 128, 255); // CYAN/BLUE?
    colorMap[COLOR_LGREEN] = RGB888(64, 255, 64);
    colorMap[COLOR_DGREEN] = RGB888(0, 192, 0);
  }
  setPaletteItem(COLOR_BLACK, colorMap[COLOR_BLACK]);
  setPaletteItem(COLOR_GREEN, colorMap[COLOR_GREEN]);
  setPaletteItem(COLOR_YELLOW, colorMap[COLOR_YELLOW]);
  setPaletteItem(COLOR_BLUE, colorMap[COLOR_BLUE]);
  setPaletteItem(COLOR_RED, colorMap[COLOR_RED]);
  setPaletteItem(COLOR_BUFF, colorMap[COLOR_BUFF]);
  setPaletteItem(COLOR_CYAN, colorMap[COLOR_CYAN]);
  setPaletteItem(COLOR_MAGENTA, colorMap[COLOR_MAGENTA]);

  // setPaletteItem(COLOR_ORANGE, colorMap[COLOR_ORANGE]);
  // setPaletteItem(COLOR_CYANBLUE, colorMap[COLOR_CYANBLUE]);
  // setPaletteItem(COLOR_LGREEN, colorMap[COLOR_LGREEN]);
  // setPaletteItem(COLOR_DGREEN, colorMap[COLOR_DGREEN]);

  // Semi Graphic
  semiColorTbl[0] = COLOR_BLACK;
  semiColorTbl[1] = COLOR_GREEN;
  semiColorTbl[2] = COLOR_YELLOW;
  semiColorTbl[3] = COLOR_BLUE;
  semiColorTbl[4] = COLOR_RED;
  semiColorTbl[5] = COLOR_BUFF;
  semiColorTbl[6] = COLOR_CYAN;
  semiColorTbl[7] = COLOR_MAGENTA;
  semiColorTbl[8] = COLOR_ORANGE;

  // Text Mode
  colorTbl[0][0] = COLOR_BLACK;
  colorTbl[0][1] = COLOR_GREEN;
  colorTbl[0][2] = COLOR_BUFF;
  colorTbl[0][3] = COLOR_ORANGE;

  // Screen 2
  colorTbl[2][0] = COLOR_GREEN;
  colorTbl[2][1] = COLOR_YELLOW;
  colorTbl[2][2] = COLOR_BLUE;
  colorTbl[2][3] = COLOR_RED;

  // Screen 3
  colorTbl[3][0] = COLOR_BLUE;
  colorTbl[3][1] = COLOR_CYANBLUE;
  colorTbl[3][2] = COLOR_MAGENTA;
  colorTbl[3][3] = COLOR_ORANGE;

  // Screen 4
  colorTbl[4][0] = COLOR_BLACK;
  colorTbl[4][1] = COLOR_GREEN;

  // Screen 5
  colorTbl[5][0] = COLOR_BLACK;
  colorTbl[5][1] = COLOR_LGREEN;

  switch (colorMode) {
  case CM_OLDREV:
    colorTbl[0][2] = COLOR_BLACK;
    colorTbl[0][3] = COLOR_CYAN;

    colorTbl[5][1] = COLOR_CYAN;
    break;
  case CM_NEW1:
    colorTbl[0][2] = COLOR_BLACK;
    colorTbl[0][3] = COLOR_ORANGE;
    break;
  case CM_NEW2:
    break;
  case CM_GREEN:
    colorTbl[2][0] = COLOR_DGREEN;
    break;
  }
}
