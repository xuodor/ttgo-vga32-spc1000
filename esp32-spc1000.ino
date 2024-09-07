#include "freertos/FreeRTOS.h"
#include "src/spc1000.h"

SPC1000 spc;
TaskHandle_t vdgTaskHandle;
TaskHandle_t cpuTaskHandle;
TaskHandle_t psgTaskHandle;

void vdgTask(void *params) {
  Serial.printf("VDGTask: core: %d\n", xPortGetCoreID());
  MC6847 *vdg = (MC6847 *)params;
  while (true) {
    vdg->RefreshScreen();
    vTaskDelay(32/portTICK_PERIOD_MS);
  }
}

void cpuTask(void *params) {
  Serial.printf("CPUTask: core: %d\n", xPortGetCoreID());
  SPC1000 *spc = (SPC1000 *)params;
  spc->Run();
}

#define PSG_PERIOD 1000

void psgTask(void *params) {
  Serial.printf("PSGTask: core: %d\n", xPortGetCoreID());
  AY38910 *psg = (AY38910 *)params;
//  psg->Init();
  while (true) {
//    psg->Loop(1);
    vTaskDelay(1/portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  spc.Init();
  xTaskCreatePinnedToCore(vdgTask, "VDG", 16*1024, spc.vdg(), 2, &vdgTaskHandle, 0);
  configASSERT(vdgTaskHandle);

  xTaskCreatePinnedToCore(cpuTask, "CPU", 16*1024, &spc, 2, &cpuTaskHandle, 1);
  configASSERT(cpuTaskHandle);

//  xTaskCreatePinnedToCore(psgTask, "PSG", 4*1024, spc.psg(), 1, &psgTaskHandle, 0);
//  configASSERT(psgTaskHandle);
}

void loop() {
}
