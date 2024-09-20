#include "dispdrivers/vga8controller.h"

#define PAGE_SIZE 32*16

class MC6847 : public fabgl::VGA8Controller {

public:

  MC6847();
  ~MC6847() = default;

  MC6847(MC6847 const&) = delete;
  void operator=(MC6847 const&) = delete;
  void Init(uint8_t *iomem);
  void RefreshScreen();

  bool screen_mode() { return 0; }
  void SavePage();
  void RestorePage();
  byte *text_pos(int x, int y);

private:
  int page_;
  uint8_t *iomem_;
  uint8_t *page_buf_;
  uint8_t *font_internal_;
};
