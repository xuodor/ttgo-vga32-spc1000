#include <stdio.h>
#include <stdarg.h>
#include "sysdep.h"
#include "fabutils.h"

static fabgl::FileBrowser *fs_;

void init_fs() {
  if (!fabgl::FileBrowser::mountSDCard(false, SD_MOUNT_PATH)) {
    Serial.printf("Mounting sd failed.");
  }
}

fabgl::FileBrowser *fs() {
  if (!fs_) {
    fs_ = new fabgl::FileBrowser(SD_MOUNT_PATH);
  }
  return fs_;
}

FILE *ext_fopen(const char *pathname, const char *mode) {
  return fs()->openFile(pathname, mode);
}

void ext_remove(const char *pathname) {
  fs()->remove(pathname);
}

uint32_t get_timestamp_ms() {
  return esp_timer_get_time()/1000;
}
