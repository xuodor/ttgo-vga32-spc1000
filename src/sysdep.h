#pragma once

#include <stdio.h>
#include <stdint.h>
#include "fabutils.h"
#include "common.h"
#include "glob.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SD_MOUNT_PATH "/SD"

extern int screenMode;
extern byte *cgbuf_;
extern int currentPage;
extern byte *vram_;

fabgl::FileBrowser *fs();
FILE *ext_fopen(const char *pathname, const char *mode);
uint32_t get_timestamp_ms();
uint32_t cas_start_time();
int serial_printf(const char *s);
#ifdef __cplusplus
}
#endif
