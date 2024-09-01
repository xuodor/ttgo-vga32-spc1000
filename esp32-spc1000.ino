#include "freertos/FreeRTOS.h"
#include "src/spc1000.h"

SPC1000 spc;
TaskHandle_t vdgTaskHandle;

void vdgTask(void *params) {
  Serial.printf("VDGTask: core: %d\n", xPortGetCoreID());
  MC6847 *vdg = (MC6847 *)params;
  while (true) {
    vdg->RefreshScreen();
    vTaskDelay(32/portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  spc.Init();
  xTaskCreate(vdgTask, "VDG", 16*1024, spc.vdg(), 2, &vdgTaskHandle);
  configASSERT(vdgTaskHandle);
}

void loop() {
  Serial.printf("CPUTask: core: %d\n", xPortGetCoreID());
  spc.Run();
}
