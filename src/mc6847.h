#include "dispdrivers/vga8controller.h"
#include "fabgl.h"

class MC6847 : public fabgl::VGA8Controller {

public:

  MC6847();
  ~MC6847() = default;

  MC6847(MC6847 const&) = delete;
  void operator=(MC6847 const&) = delete;
  void Init(byte *iomem);
  void SetMode(int command, byte param);
  void RefreshScreen();
  void SavePage();
  void RestorePage();
  byte *text_pos(int x, int y);
  bool mode() const { return mode_; }

private:
  void InitColor(int mode);
  void GetFontData(byte code, byte attr,
                   int *fgColor, int *bgColor, byte **fontData);
  void RefreshTextSemiGraphic();
  void RefreshMidGraphic();
  void RefreshHiGraphic();

  int page_;
  int mode_;
  byte *iomem_;
  byte *page_buf_;
  byte *font_internal_;
  RGB888 rgb_[12];
  int color_tbl_[6][4];
  byte semigr_font_0[16*12]; // semigraphic pattern for mode 0
  byte semigr_font_1[64*12]; // semigraphic pattern for mode 1
  int semigr_tbl_[9]; // Color Table for semigraphic
  void (MC6847::*refresh_func_)();
};
