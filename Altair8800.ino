#include <string>

#include "fabgl.h"
#include "fabutils.h"
#include "src/machine.h"

Machine spc;

void setup() {

  Serial.begin(115200);

  spc.init();
  spc.setRealSpeed(true);
  spc.run();
}

void loop() {

}
