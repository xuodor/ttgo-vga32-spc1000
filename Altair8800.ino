#include <string>

#include "fabgl.h"
#include "fabutils.h"
#include "src/machine.h"

fabgl::VGA8Controller displayController;

Machine spc;

void setup() {

  Serial.begin(115200);

  displayController.begin();
  displayController.setResolution(VGA_640x480_60Hz);

  displayController.setPaletteItem(0, RGB888(0, 0, 0));
  displayController.setPaletteItem(1, RGB888(255, 0, 0));
  displayController.setPaletteItem(2, RGB888(0, 255, 0));
  displayController.setPaletteItem(3, RGB888(0, 0, 255));
  displayController.setPaletteItem(4, RGB888(255, 255, 0));
  displayController.setPaletteItem(5, RGB888(255, 0, 255));
  displayController.setPaletteItem(6, RGB888(0, 255, 255));
  displayController.setPaletteItem(7, RGB888(255, 255, 255));

  for (int y = 0; y < 480; ++y) {
    for (int x = 0; x < 640; ++x) {
      if (!(48 <= y && y < 480-64 && 64 <= x && x < 640-64)) {
        displayController.directSetPixel(x, y, 2);
      }
    }
  }
  spc.setRealSpeed(true);
  spc.load(0, spcrom, 8);
  spc.run();
}

void loop() {

}
