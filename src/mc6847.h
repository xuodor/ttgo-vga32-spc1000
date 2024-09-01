#pragma once

#include "dispdrivers/vga8controller.h"

class MC6847 : public fabgl::VGA8Controller {

public:

  MC6847();
  ~MC6847() = default;

  MC6847(MC6847 const&) = delete;
  void operator=(MC6847 const&) = delete;
  void Init(uint8_t *iomem);
  void RefreshScreen();

private:
  int current_page_;
  uint8_t *iomem_;
  uint8_t *font_internal_;
};
