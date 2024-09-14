#include "freertos/FreeRTOS.h"
#include "src/spc1000.h"

#define DIR_PATH "/SD"

SPC1000 spc;
fabgl::FileBrowser *file_browser_;

TaskHandle_t vdgTaskHandle;
TaskHandle_t cpuTaskHandle;

void vdgTask(void *params) {
  MC6847 *vdg = (MC6847 *)params;
  while (true) {
    vdg->RefreshScreen();
    vTaskDelay(256/portTICK_PERIOD_MS);
  }
}

void cpuTask(void *params) {
  SPC1000 *spc = (SPC1000 *)params;
  spc->Run();
}

#define FORMAT_ON_FAIL     false
#define SDCARD_MOUNT_PATH  "/SD"

void setup() {
  Serial.begin(115200);
  spc.Init();

  xTaskCreatePinnedToCore(vdgTask, "VDG", 4*1024, spc.vdg(), 2, &vdgTaskHandle, 0);
  configASSERT(vdgTaskHandle);
/*
  if (!fabgl::FileBrowser::mountSDCard(FORMAT_ON_FAIL, SDCARD_MOUNT_PATH)) {
    Serial.printf("Mounting sd failed.");
    return;
  }

  file_browser_ = new fabgl::FileBrowser();
  if (!file_browser_->setDirectory(DIR_PATH)) {
    Serial.printf("Setting disk directory failed.");
    return;
  }
*/
  xTaskCreatePinnedToCore(cpuTask, "CPU", 32*1024, &spc, 2, &cpuTaskHandle, 1);
  configASSERT(cpuTaskHandle);
}

void loop() {
}
