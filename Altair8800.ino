#include <string>

#include <Preferences.h>

#include "fabgl.h"
#include "fabutils.h"

#include "src/machine.h"



// Flash and SDCard configuration
#define FORMAT_ON_FAIL     true
#define SPIFFS_MOUNT_PATH  "/flash"
#define SDCARD_MOUNT_PATH  "/SD"


// Display controller (textual or bitmapped)
//#define USE_TEXTUAL_DISPLAYCONTROLLER


// consts

constexpr int DefaultTermIndex   = 2;   // Default: "ADM-31"
constexpr int MaxTermIndex       = 7;   // Max: "Legacy ANSI"

constexpr int DefaultKbdLayIndex = 2;   // Default: "UK"

constexpr int DefaultRowsCount   = 0;   // 0 = no limit (34 on the text terminal)

const char * ColorsStr[]         = { "Green/Black",      "Yellow/Black",      "White/Black",      "Black/White",      "Yellow/Blue",       "Black/Yellow" };
const Color TextColors[]         = { Color::BrightGreen, Color::BrightYellow, Color::BrightWhite, Color::Black,       Color::BrightYellow, Color::Black };
const Color BackColors[]         = { Color::Black,       Color::Black,        Color::Black,       Color::BrightWhite, Color::Blue,         Color::BrightYellow };
constexpr int DefaultColorsIndex = 0;   // Default: Green/Black
constexpr int MaxColorsIndex     = 5;


fabgl::PS2Controller     PS2Controller;
fabgl::Terminal          Terminal;

Machine              altair;
Preferences          preferences;


// base path (can be SPIFFS_MOUNT_PATH or SDCARD_MOUNT_PATH depending from what was successfully mounted first)
char const * basepath = nullptr;


using std::string;


fabgl::VGADirectController DisplayController;


volatile double      objX                 = 300;
volatile double      objY                 = 200;
static double        objDir               = 0.3;
static double        objVel               =  10;
static constexpr int objSize              =  25;
static uint32_t      bgParkMillerState    = 1;
static constexpr int borderSize           = 48;
static constexpr int hborder = 64;
constexpr int        scanlinesPerCallback = 2;  // screen height should be divisible by this value

static TaskHandle_t  mainTaskHandle;


// just to avoid floating point calculations inside MyDirectDrawVGAController::drawScanline()
volatile int objIntX;
volatile int objIntY;


inline int fastRandom()
{
  bgParkMillerState = (uint64_t)bgParkMillerState * 48271 % 0x7fffffff;
  return bgParkMillerState % 4;
}


void IRAM_ATTR drawScanline(void * arg, uint8_t * dest, int scanLine)
{
  auto fgcolor = DisplayController.createRawPixel(RGB222(3, 0, 0)); // red
  auto bgcolor = DisplayController.createRawPixel(RGB222(0, 0, 2)); // blue
  auto borderColor = DisplayController.createRawPixel(RGB222(0, 0, 0)); // black

  auto width  = DisplayController.getScreenWidth();
  auto height = DisplayController.getScreenHeight();

  // draws "scanlinesPerCallback" scanlines every time drawScanline() is called
  for (int i = 0; i < scanlinesPerCallback; ++i) {

    // fill upper and lower border with random background color
    if (scanLine < borderSize || scanLine > height - borderSize)
      bgcolor = borderColor;

    // fill line with background color
    memset(dest, bgcolor, width);

    for (int col = 0; col < hborder; ++col) {
        VGA_PIXELINROW(dest, col) = borderColor;
        VGA_PIXELINROW(dest, width-col-1) = borderColor;
    }
    // fill object with foreground color
    if (scanLine >= objIntY - objSize / 2 && scanLine <= objIntY + objSize / 2) {
      for (int col = objIntX - objSize / 2; col < objIntX + objSize / 2; ++col) {
        if (col >= 0 && col < width) {
          VGA_PIXELINROW(dest, col) = fgcolor;
        }
      }
    }

    // go to next scanline
    ++scanLine;
    dest += width;

  }

  if (scanLine == height) {
    // signal end of screen
    vTaskNotifyGiveFromISR(mainTaskHandle, NULL);
  }
}

TermType getTerminalEmu()
{
  return (TermType) preferences.getInt("termEmu", DefaultTermIndex);
}


void setupTerminalEmu()
{
  Terminal.setTerminalType(getTerminalEmu());
}


// 0 = max
int getRowsCount()
{
  return preferences.getInt("rowsCount", DefaultRowsCount);
}


void setupRowsCount()
{
  int rows = getRowsCount();
  if (rows <= 0)
    Terminal.write("\e[r");             // disable scrolling region
  else
    Terminal.printf("\e[1;%dr", rows);  // enable scrolling region (1..rows)
}


int getKbdLayoutIndex()
{
  return preferences.getInt("kbdLay", DefaultKbdLayIndex);
}


void setupKbdLayout()
{
  PS2Controller.keyboard()->setLayout( SupportedLayouts::layouts()[getKbdLayoutIndex()] );
}


bool getRealCPUSpeed()
{
  return preferences.getBool("realSpeed", false);
}


void setupRealCPUSpeed()
{
  altair.setRealSpeed(getRealCPUSpeed());
}


bool getEmuCRT()
{
  return preferences.getBool("emuCRT", false);
}


int getColorsIndex()
{
  return preferences.getInt("colors", DefaultColorsIndex);
}


void setupTerminalColors()
{
  int colorsIndex = getColorsIndex();
  Terminal.setForegroundColor(TextColors[colorsIndex]);
  Terminal.setBackgroundColor(BackColors[colorsIndex]);
}


void setup()
{
  preferences.begin("altair8800", false);

  Serial.begin(115200);

  // setup Keyboard (default configuration)
  PS2Controller.begin(PS2Preset::KeyboardPort0);

  mainTaskHandle = xTaskGetCurrentTaskHandle();

  DisplayController.begin();
  DisplayController.setScanlinesPerCallBack(scanlinesPerCallback);
  DisplayController.setDrawScanlineCallback(drawScanline);
  DisplayController.setResolution(VGA_640x480_60Hz);

}


void loop()
{
  // RAM
  altair.attachRAM(65536);

  // boot ROM
  altair.load(0, Altair88DiskBootROM, sizeof(Altair88DiskBootROM));

  // setup keyboard layout
  setupKbdLayout();

  // CPU speed
  setupRealCPUSpeed();

  // test collision with borders and bounce changing direction
  if (objX < objSize / 2 || objX > DisplayController.getScreenWidth() - objSize / 2)
    objDir = PI - objDir;
  else if (objY < borderSize + objSize / 2 || objY > DisplayController.getScreenHeight() - borderSize - objSize / 2)
    objDir = 2 * PI - objDir;

  // calculate new coordinates
  objX += objVel * cos(objDir);
  objY += objVel * sin(objDir);

  // convert object coordinate to integer
  objIntX = objX;
  objIntY = objY;


  altair.run(0x0000);
}
