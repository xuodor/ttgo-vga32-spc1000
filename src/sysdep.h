#pragma once

#include <stdio.h>
#include <stdint.h>
#include "fabutils.h"
#include "common.h"
#include "glob.h"

#define SD_MOUNT_PATH "/SD"

#ifdef __cplusplus
extern "C" {
#endif

void init_fs();
fabgl::FileBrowser *fs();
FILE *ext_fopen(const char *pathname, const char *mode);
uint32_t get_timestamp_ms();
uint32_t cas_start_time();
#ifdef __cplusplus
}
#endif
