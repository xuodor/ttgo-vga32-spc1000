/*
  Created by Fabrizio Di Vittorio (fdivitto2013@gmail.com) - <http://www.fabgl.com>
  Copyright (c) 2019-2022 Fabrizio Di Vittorio.
  All rights reserved.


* Please contact fdivitto2013@gmail.com if you need a commercial license.


* This library and related software is available under GPL v3.

  FabGL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  FabGL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with FabGL.  If not, see <http://www.gnu.org/licenses/>.
 */



 /*
  * Optional SD Card connections:
  *   MISO => GPIO 16  (2 for PICO-D4)
  *   MOSI => GPIO 17  (12 for PICO-D4)
  *   CLK  => GPIO 14
  *   CS   => GPIO 13
  *
  * To change above assignment fill other paramaters of FileBrowser::mountSDCard().
  */



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
#define USE_TEXTUAL_DISPLAYCONTROLLER


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


// globals

#ifdef USE_TEXTUAL_DISPLAYCONTROLLER
fabgl::VGATextController DisplayController;
#else
fabgl::VGA16Controller   DisplayController;
#endif
fabgl::PS2Controller     PS2Controller;
fabgl::Terminal          Terminal;

Machine              altair;
SIO                  SIO0(&altair, 0x00);
SIO                  SIO1(&altair, 0x10);
SIO                  SIO2(&altair, 0x12);
Preferences          preferences;


// base path (can be SPIFFS_MOUNT_PATH or SDCARD_MOUNT_PATH depending from what was successfully mounted first)
char const * basepath = nullptr;


using std::string;


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


// shown pressing PAUSE
void emulator_menu()
{
  bool resetRequired = false;
  bool changedRowsCount = false;

  for (bool loop = true; loop; ) {


    Terminal.setTerminalType(TermType::ANSILegacy);

    Terminal.write("\r\n\n\e[97m\e[40m                            ** Emulator Menu **\e[K\r\n\e[K\n");
    Terminal.write( "\e[93m Z \e[37m Reset\e[K\n\r");
    Terminal.write( "\e[93m S \e[37m Send Disk to Serial\e[K\n\r");
    Terminal.write( "\e[93m R \e[37m Get Disk from Serial\e[K\n\r");
    Terminal.write( "\e[93m F \e[37m Format FileSystem\e[K\n\r");
    Terminal.printf("\e[93m P \e[37m Real CPU Speed: \e[33m%s\e[K\n\r", getRealCPUSpeed() ? "YES" : "NO");
    Terminal.write("\e[6A");  // cursor UP
    Terminal.printf("\t\t\t\t\t\e[93m T \e[37m Terminal: \e[33m%s\e[K\n\r", SupportedTerminals::names()[(int)getTerminalEmu()] );
    Terminal.printf("\t\t\t\t\t\e[93m K \e[37m Keyboard Layout: \e[33m%s\e[K\n\r", SupportedLayouts::names()[getKbdLayoutIndex()] );
    #ifndef USE_TEXTUAL_DISPLAYCONTROLLER
    Terminal.printf("\t\t\t\t\t\e[93m G \e[37m CRT Mode: \e[33m%s\e[K\n\r", getEmuCRT() ? "YES" : "NO");
    #endif
    Terminal.printf("\t\t\t\t\t\e[93m C \e[37m Colors: \e[33m%s\e[K\n\r", ColorsStr[getColorsIndex()] );
    Terminal.printf("\t\t\t\t\t\e[93m L \e[37m Rows: \e[33m%d\e[K\n\r",  getRowsCount() <= 0 ? Terminal.getRows() : getRowsCount());
    Terminal.write( "\t\t\t\t\t\e[93m O \e[37m Reset Configuration\e[K\n\r");

    Terminal.write("\n\n\e[97mPlease select an option (ENTER to exit): \e[K\e[92m");
    int ch = toupper(Terminal.read());
    Terminal.write(ch);
    Terminal.write("\n\r");
    switch (ch) {
      // Format FileSystem
      case 'F':
        Terminal.write("\e[91mFormat deletes all disks and files in your SD or SPIFFS!\e[92m\r\nAre you sure? (Y/N)");
        if (toupper(Terminal.read()) == 'Y') {
          Terminal.write("\n\rFormatting...");
          Terminal.flush();
          FileBrowser::format(FileBrowser::getDriveType(basepath), 0);
          resetRequired = true;
        }
        break;

      // Reset
      case 'Z':
        Terminal.write("Resetting the Altair. Are you sure? (Y/N)");
        if (toupper(Terminal.read()) == 'Y')
          ESP.restart();
        break;

      // Real CPU speed
      case 'P':
        preferences.putBool("realSpeed", !getRealCPUSpeed());
        setupRealCPUSpeed();
        break;

      // Emulate CRT
      case 'G':
        preferences.putBool("emuCRT", !getEmuCRT());
        resetRequired = true;
        break;

      // Terminal emulation
      case 'T':
      {
        int termIndex = (int)getTerminalEmu() + 1;
        if (termIndex > MaxTermIndex)
          termIndex = 0;
        preferences.putInt("termEmu", termIndex);
        break;
      }

      // Keyboard layout
      case 'K':
      {
        int kbdLayIndex = getKbdLayoutIndex() + 1;
        if (kbdLayIndex >= SupportedLayouts::count())
          kbdLayIndex = 0;
        preferences.putInt("kbdLay", kbdLayIndex);
        setupKbdLayout();
        break;
      }

      // Colors
      case 'C':
      {
        int colorsIndex = getColorsIndex() + 1;
        if (colorsIndex > MaxColorsIndex)
          colorsIndex = 0;
        preferences.putInt("colors", colorsIndex);
        break;
      }

      // Rows
      case 'L':
      {
        int rows = getRowsCount() + 1;
        if (rows < 24)
          rows = 24;  // min
        if (rows > 25)
          rows = 0;   // max
        preferences.putInt("rowsCount", rows);
        changedRowsCount = true;
        break;
      }

      // Reset configuration
      case 'O':
        preferences.clear();
        resetRequired = true;
        break;

      default:
        loop = false;
        break;

    }
  }

  if (resetRequired) {
    Terminal.write("Reset required. Reset now? (Y/N)");
    if (toupper(Terminal.read()) == 'Y') {
      ESP.restart();
    }
  }

  Terminal.localWrite("\n");

  if (changedRowsCount) {
    setupRowsCount();
    Terminal.clear();
    Terminal.write("Clear screen required. Settings applied.\r\n\e\n");
  }

  setupTerminalColors();
  setupTerminalEmu();
}


void setup()
{
  preferences.begin("altair8800", false);

  Serial.begin(115200);

  // setup Keyboard (default configuration)
  PS2Controller.begin(PS2Preset::KeyboardPort0);

  // setup VGA (default configuration with 64 colors)
  DisplayController.begin();

  if (getEmuCRT())
    DisplayController.setResolution(VGA_640x200_70HzRetro);
  else
    DisplayController.setResolution(VGA_640x200_70Hz);

  Terminal.begin(&DisplayController);
  Terminal.connectLocally();      // to use Terminal.read(), available(), etc..

  Terminal.setBackgroundColor(Color::Black);
  Terminal.setForegroundColor(Color::BrightGreen);
  Terminal.clear();

  Terminal.enableCursor(true);
}


void loop()
{
  if (FileBrowser::mountSDCard(FORMAT_ON_FAIL, SDCARD_MOUNT_PATH))
    basepath = SDCARD_MOUNT_PATH;
  else if (FileBrowser::mountSPIFFS(FORMAT_ON_FAIL, SPIFFS_MOUNT_PATH))
    basepath = SPIFFS_MOUNT_PATH;

  // setup disk drives

  // setup SIOs (Serial I/O)

  // TTY
  SIO0.attachStream(&Terminal);

  // CRT/Keyboard
  SIO1.attachStream(&Terminal);

  // Serial
  SIO2.attachStream(&Serial);

  // RAM
  altair.attachRAM(65536);

  // boot ROM
  altair.load(Altair88DiskBootROMAddr, Altair88DiskBootROM, sizeof(Altair88DiskBootROM));

  // menu callback (pressing PAUSE or F12)
  altair.setMenuCallback(emulator_menu);

  setupTerminalColors();
  Terminal.clear();

  setupRowsCount();

  Terminal.write("\e[97m\e[44m");
  Terminal.write("                    /* * * * * * * * * * * * * * * * * * * *\e[K\r\n");
  Terminal.write("                             A L T A I R     8 8 0 0 \e[K\r\n");
  Terminal.write("                     \e[37mby Fabrizio Di Vittorio - www.fabgl.com\e[97m\e[K\r\n");
  Terminal.write("                     * * * * * * * * * * * * * * * * * * * */\e[K\r\n\e[K\n");

  Terminal.printf("\e[33mFree Memory :\e[32m %d bytes\e[K\r\n", heap_caps_get_free_size(MALLOC_CAP_32BIT));

  int64_t total, used;
  FileBrowser::getFSInfo(FileBrowser::getDriveType(basepath), 0, &total, &used);
  Terminal.printf("\e[33mFile System :\e[32m %lld KiB used, %lld KiB free\e[K\r\n", used / 1024, (total - used) / 1024);

  Terminal.printf("\e[33mKbd Layout  : \e[32m%s\e[K\r\n", SupportedLayouts::names()[getKbdLayoutIndex()] );

  Terminal.printf("Press \e[93m[F12]\e[92m or \e[93m[PAUSE]\e[92m to display emulator menu\e[K\r\n");

  setupTerminalColors();

  Terminal.setColorForAttribute(CharStyle::Bold, Color::BrightYellow, false);
  Terminal.setColorForAttribute(CharStyle::Italic, Color::BrightRed, false);
  Terminal.setColorForAttribute(CharStyle::Underline, Color::BrightWhite, false);
  Terminal.setColorForAttribute(CharStyle::ReducedLuminosity, Color::BrightYellow, false);

  // setup keyboard layout
  setupKbdLayout();

  // setup terminal emulation
  setupTerminalEmu();

  // CPU speed
  setupRealCPUSpeed();

  altair.run(Altair88DiskBootROMRun);
}
