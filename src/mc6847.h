#include "dispdrivers/vga8controller.h"
#include "fabgl.h"

#define PAGE_SIZE 32*16

#define GM_TEXT     0
#define GM_GRAPHIC  1

class MC6847 : public fabgl::VGA8Controller {

public:

  MC6847();
  ~MC6847() = default;

  MC6847(MC6847 const&) = delete;
  void operator=(MC6847 const&) = delete;
  void Init(uint8_t *iomem);
  void SetMode(int command, byte param);
  void SavePage();
  void RestorePage();
  byte *text_pos(int x, int y);
  bool screen_mode() { return 0; }
  void RefreshScreen();

private:
  void InitColor(int mode);
  void GetFontData(byte code, byte attr,
                   int *fgColor, int *bgColor, byte **fontData);
  void RefreshTextSemiGraphic();
  void RefreshMidGraphic();
  void RefreshHiGraphic();

  int page_;
  int mode_;
  uint8_t *iomem_;
  uint8_t *page_buf_;
  uint8_t *font_internal_;
  RGB888 rgb_[12];
  byte semiGrFont0[16 * 12]; // semigraphic pattern for mode 0
  byte semiGrFont1[64 * 12]; // semigraphic pattern for mode 1
  int colorTbl[6][4];  // Palette for text and mid,hi-res graphics
  int semiColorTbl[9]; // Color Table for semigraphic
  void (MC6847::*refresh_func_)();
};
