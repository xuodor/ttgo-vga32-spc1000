#include "freertos/FreeRTOS.h"
#include "src/machine.h"

Machine spc1000;
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
  spc1000.init();
  spc1000.setRealSpeed(true);
  xTaskCreate(vdgTask, "VDG", 16*1024, spc1000.vdg(), 2, &vdgTaskHandle);
  configASSERT(vdgTaskHandle);
}

void loop() {
  Serial.printf("CPUTask: core: %d\n", xPortGetCoreID());
  spc1000.run();
}
