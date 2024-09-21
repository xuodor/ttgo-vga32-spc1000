#include "dispdrivers/vga8controller.h"
#include "fabgl.h"

#define PAGE_SIZE 32*16

#define CM_OLDREV   0
#define CM_NEW1     1
#define CM_NEW2     2
#define CM_GREEN    3

#define GM_TEXT     0
#define GM_GRAPHIC  1

enum colorNum {
  COLOR_BLACK = 0,
  COLOR_GREEN = 1,
  COLOR_YELLOW = 2,
  COLOR_BLUE = 3,
  COLOR_RED = 4,
  COLOR_BUFF = 5,
  COLOR_CYAN = 6,
  COLOR_MAGENTA = 7,
  COLOR_ORANGE = 8,
  COLOR_CYANBLUE = 9,
  COLOR_LGREEN = 10,
  COLOR_DGREEN = 11
};

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
  RGB888 colorMap[12];
  byte semiGrFont0[16 * 12]; // semigraphic pattern for mode 0
  byte semiGrFont1[64 * 12]; // semigraphic pattern for mode 1
  int colorTbl[6][4];  // Palette for text and mid,hi-res graphics
  int semiColorTbl[9]; // Color Table for semigraphic
  void (MC6847::*refresh_func_)();
};
