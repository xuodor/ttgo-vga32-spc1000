#include <stdio.h>
#include <stdarg.h>
#include "sysdep.h"
#include "fabutils.h"

int currentPage;
byte *vram_;

void init_fs() {
}

FILE *ext_fopen(const char *pathname, const char *mode) {
  fabgl::FileBrowser fb(SD_MOUNT_PATH);
  return fb.openFile(pathname, mode);
}

uint32_t get_timestamp_ms() {
  return esp_timer_get_time()/1000;
}
