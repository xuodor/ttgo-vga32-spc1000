#include "freertos/FreeRTOS.h"
#include "src/spc1000.h"

SPC1000 spc;

TaskHandle_t vdgTaskHandle;
TaskHandle_t cpuTaskHandle;

void vdgTask(void *params) {
  MC6847 *vdg = (MC6847 *)params;
  while (true) {
    vdg->RefreshScreen();
    vTaskDelay(32/portTICK_PERIOD_MS);
  }
}

void cpuTask(void *params) {
  SPC1000 *spc = (SPC1000 *)params;
  spc->Run();
}

void setup() {
  Serial.begin(115200);
  spc.Init();
  xTaskCreatePinnedToCore(vdgTask, "VDG", 4*1024, spc.vdg(), 2, &vdgTaskHandle, 0);
  configASSERT(vdgTaskHandle);

  xTaskCreatePinnedToCore(cpuTask, "CPU", 32*1024, &spc, 2, &cpuTaskHandle, 1);
  configASSERT(cpuTaskHandle);
}

void loop() {
}
