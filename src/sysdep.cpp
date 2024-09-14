#include <stdio.h>
#include <stdarg.h>
#include "sysdep.h"
#include "fabutils.h"

int currentPage;
byte *vram_;
extern fabgl::FileBrowser *file_browser_;

void init_fs() {
}

FILE *ext_fopen(const char *pathname, const char *mode) {
  return file_browser_->openFile(pathname, mode);
}

uint32_t get_timestamp_ms() {
  return esp_timer_get_time()/1000;
}
