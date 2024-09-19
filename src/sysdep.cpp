#include <stdio.h>
#include <stdarg.h>
#include "sysdep.h"
#include "fabutils.h"

int currentPage;
byte *vram_;

static fabgl::FileBrowser *fs_;
void init_fs() {
}

fabgl::FileBrowser *fs() {
  if (!fs_) {
    fs_ = new fabgl::FileBrowser();
    fs_->setDirectory(SD_MOUNT_PATH);
  }
  return fs_;
}

FILE *ext_fopen(const char *pathname, const char *mode) {
  return fs()->openFile(pathname, mode);
}

uint32_t get_timestamp_ms() {
  return esp_timer_get_time()/1000;
}
