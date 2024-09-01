#include <sys/types.h>

#pragma once

#include "dispdrivers/vga8controller.h"

class MC6847 : public fabgl::VGA8Controller {

public:

  MC6847();
  ~MC6847() = default;

  MC6847(MC6847 const&) = delete;
  void operator=(MC6847 const&) = delete;
  void InitVRAM(uint8_t *mem);
  void RefreshScreen();

private:
  int current_page_;
  uint8_t *vram_;
  uint8_t *font_internal_;
  const int kOffsetX_ = 64;
  const int kOffsetY_ = 48;
};
