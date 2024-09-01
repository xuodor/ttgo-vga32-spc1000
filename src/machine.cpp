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



#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

#include "machine.h"

#include "fabgl.h"
#include "fabutils.h"

extern uint8_t rom[];
/*
const uint8_t spcrom[0x80] = {
  0xed, 0x56, 0xfb, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x2a, 0x70, 0x00, 0x23, 0x22, 0x70, 0x00, 0x00,

  0x01, 0x03, 0x20, 0x3e, 0xaa, 0xed, 0x79, 0x00,
  0x01, 0x02, 0x30, 0x3e, 0x55, 0xed, 0x79, 0x00,

  0x01, 0x02, 0x40, 0x3e, 0xaa, 0xed, 0x79, 0x00,
  0x01, 0x02, 0x80, 0x3e, 0x55, 0xed, 0x79, 0x00,

  0xc3, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xc3, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0xe5, 0x2a, 0x36, 0x00,
  0x23, 0x22, 0x36, 0x00, 0xe1, 0xfb, 0xed, 0x4d
};
*/

////////////////////////////////////////////////////////////////////////////////////
// Machine

Machine::Machine()
  : m_devices(nullptr),
    m_realSpeed(false)
{
  m_Z80.setCallbacks(this, readByte, writeByte, readWord, writeWord, readIO, writeIO);
}


Machine::~Machine()
{

}


void Machine::attachDevice(Device * device)
{
  device->next = m_devices;
  m_devices = device;
}


void Machine::init() {
  memset(m_RAM, 0, 65536);

  load(0, rom, 0x8000);
  mc6847.begin();
  mc6847.setResolution(VGA_640x480_60Hz);
  mc6847.InitVRAM(vram_);

  mc6847.setPaletteItem(0, RGB888(0, 0, 0));
  mc6847.setPaletteItem(1, RGB888(255, 0, 0));
  mc6847.setPaletteItem(2, RGB888(0, 255, 0));
  mc6847.setPaletteItem(3, RGB888(0, 0, 255));
  mc6847.setPaletteItem(4, RGB888(255, 255, 0));
  mc6847.setPaletteItem(5, RGB888(255, 0, 255));
  mc6847.setPaletteItem(6, RGB888(0, 255, 255));
  mc6847.setPaletteItem(7, RGB888(255, 255, 255));

  keyboard_.begin(PS2Preset::KeyboardPort0);

  for (int i = fabgl::VK_NONE; i < fabgl::VK_LAST; ++i) {
    key_table_[i] = { -1, 0 };
  }
  key_table_[fabgl::VK_RSHIFT] = { 0, 0x02 };
  key_table_[fabgl::VK_LSHIFT] = { 0, 0x02 };
  key_table_[fabgl::VK_LCTRL] = { 0, 0x04 };
  key_table_[fabgl::VK_RCTRL] = { 0, 0x04 };
  key_table_[fabgl::VK_PRINTSCREEN] = { 0, 0x10 };
  key_table_[fabgl::VK_LALT] = { 0, 0x40 };
  key_table_[fabgl::VK_RALT] = { 0, 0x40 };

  key_table_[fabgl::VK_GRAVEACCENT] = { 1, 0x01 };
  key_table_[fabgl::VK_HOME] = { 1, 0x02 };
  key_table_[fabgl::VK_SPACE] = { 1, 0x04 };
  key_table_[fabgl::VK_RETURN] = { 1, 0x08 };
  key_table_[fabgl::VK_C] = { 1, 0x10 };
  key_table_[fabgl::VK_c] = { 1, 0x10 };
  key_table_[fabgl::VK_A] = { 1, 0x20 };
  key_table_[fabgl::VK_a] = { 1, 0x20 };
  key_table_[fabgl::VK_q] = { 1, 0x40 };
  key_table_[fabgl::VK_Q] = { 1, 0x40 };
  key_table_[fabgl::VK_EXCLAIM] = { 1, 0x80 };
  key_table_[fabgl::VK_1] = { 1, 0x80 };

  key_table_[fabgl::VK_TAB] = { 2, 0x01 };
  key_table_[fabgl::VK_Z] = { 2, 0x04 };
  key_table_[fabgl::VK_z] = { 2, 0x04 };
  key_table_[fabgl::VK_RIGHTBRACE] = { 1, 0x08 };
  key_table_[fabgl::VK_RIGHTBRACKET] = { 1, 0x08 };
  key_table_[fabgl::VK_V] = { 2, 0x10 };
  key_table_[fabgl::VK_v] = { 2, 0x10 };
  key_table_[fabgl::VK_S] = { 2, 0x20 };
  key_table_[fabgl::VK_s] = { 2, 0x20 };
  key_table_[fabgl::VK_W] = { 2, 0x40 };
  key_table_[fabgl::VK_w] = { 2, 0x40 };
  key_table_[fabgl::VK_AT] = { 2, 0x80 };
  key_table_[fabgl::VK_2] = { 2, 0x80 };

  key_table_[fabgl::VK_BACKSPACE] = { 3, 0x01 };
  key_table_[fabgl::VK_ESCAPE] = { 3, 0x04 };
  key_table_[fabgl::VK_LEFTBRACE] = { 3, 0x08 };
  key_table_[fabgl::VK_LEFTBRACKET] = { 3, 0x08 };
  key_table_[fabgl::VK_B] = { 3, 0x10 };
  key_table_[fabgl::VK_b] = { 3, 0x10 };
  key_table_[fabgl::VK_D] = { 3, 0x20 };
  key_table_[fabgl::VK_d] = { 3, 0x20 };
  key_table_[fabgl::VK_D] = { 3, 0x40 };
  key_table_[fabgl::VK_e] = { 3, 0x40 };
  key_table_[fabgl::VK_HASH] = { 3, 0x80 };
  key_table_[fabgl::VK_3] = { 3, 0x80 };

  key_table_[fabgl::VK_RIGHT] = { 4, 0x04 };
  key_table_[fabgl::VK_BACKSLASH] = { 4, 0x08 };
  key_table_[fabgl::VK_N] = { 4, 0x10 };
  key_table_[fabgl::VK_n] = { 4, 0x10 };
  key_table_[fabgl::VK_F] = { 4, 0x20 };
  key_table_[fabgl::VK_f] = { 4, 0x20 };
  key_table_[fabgl::VK_R] = { 4, 0x40 };
  key_table_[fabgl::VK_r] = { 4, 0x40 };
  key_table_[fabgl::VK_DOLLAR] = { 4, 0x80 };
  key_table_[fabgl::VK_4] = { 4, 0x80 };

  key_table_[fabgl::VK_F1] = { 5, 0x02 };
  key_table_[fabgl::VK_LEFT] = { 5, 0x04 };
  key_table_[fabgl::VK_M] = { 5, 0x10 };
  key_table_[fabgl::VK_m] = { 5, 0x10 };
  key_table_[fabgl::VK_G] = { 5, 0x20 };
  key_table_[fabgl::VK_g] = { 5, 0x20 };
  key_table_[fabgl::VK_Y] = { 5, 0x40 };
  key_table_[fabgl::VK_t] = { 5, 0x40 };
  key_table_[fabgl::VK_PERCENT] = { 5, 0x80 };
  key_table_[fabgl::VK_5] = { 5, 0x80 };

  key_table_[fabgl::VK_F2] = { 6, 0x02 };
  key_table_[fabgl::VK_PLUS] = { 6, 0x04 };
  key_table_[fabgl::VK_EQUALS] = { 6, 0x04 };
  key_table_[fabgl::VK_X] = { 6, 0x08 };
  key_table_[fabgl::VK_x] = { 6, 0x08 };
  key_table_[fabgl::VK_LESS] = { 6, 0x10 };
  key_table_[fabgl::VK_COMMA] = { 6, 0x10 };
  key_table_[fabgl::VK_H] = { 6, 0x20 };
  key_table_[fabgl::VK_h] = { 6, 0x20 };
  key_table_[fabgl::VK_Y] = { 6, 0x40 };
  key_table_[fabgl::VK_y] = { 6, 0x40 };
  key_table_[fabgl::VK_CARET] = { 6, 0x80 };
  key_table_[fabgl::VK_6] = { 6, 0x80 };

  key_table_[fabgl::VK_F3] = { 7, 0x02 };
  key_table_[fabgl::VK_UP] = { 7, 0x04 };
  key_table_[fabgl::VK_P] = { 7, 0x08 };
  key_table_[fabgl::VK_p] = { 7, 0x08 };
  key_table_[fabgl::VK_GREATER] = { 7, 0x10 };
  key_table_[fabgl::VK_PERIOD] = { 7, 0x10 };
  key_table_[fabgl::VK_J] = { 7, 0x20 };
  key_table_[fabgl::VK_j] = { 7, 0x20 };
  key_table_[fabgl::VK_U] = { 7, 0x40 };
  key_table_[fabgl::VK_u] = { 7, 0x40 };
  key_table_[fabgl::VK_AMPERSAND] = { 7, 0x80 };
  key_table_[fabgl::VK_7] = { 7, 0x80 };

  key_table_[fabgl::VK_F4] = { 8, 0x02 };
  key_table_[fabgl::VK_DOWN] = { 8, 0x04 };
  key_table_[fabgl::VK_QUOTEDBL] = { 8, 0x08 };
  key_table_[fabgl::VK_QUOTE] = { 8, 0x08 };
  key_table_[fabgl::VK_QUESTION] = { 8, 0x10 };
  key_table_[fabgl::VK_SLASH] = { 8, 0x10 };
  key_table_[fabgl::VK_K] = { 8, 0x20 };
  key_table_[fabgl::VK_k] = { 8, 0x20 };
  key_table_[fabgl::VK_I] = { 8, 0x40 };
  key_table_[fabgl::VK_i] = { 8, 0x40 };
  key_table_[fabgl::VK_ASTERISK] = { 8, 0x80 };
  key_table_[fabgl::VK_8] = { 8, 0x80 };

  key_table_[fabgl::VK_F5] = { 9, 0x02 };
  key_table_[fabgl::VK_MINUS] = { 9, 0x04 };
  key_table_[fabgl::VK_TILDE] = { 9, 0x04 };
  key_table_[fabgl::VK_RIGHTPAREN] = { 9, 0x08 };
  key_table_[fabgl::VK_0] = { 9, 0x08 };
  key_table_[fabgl::VK_COLON] = { 9, 0x10 };
  key_table_[fabgl::VK_SEMICOLON] = { 9, 0x10 };
  key_table_[fabgl::VK_L] = { 9, 0x20 };
  key_table_[fabgl::VK_l] = { 9, 0x20 };
  key_table_[fabgl::VK_O] = { 9, 0x40 };
  key_table_[fabgl::VK_o] = { 9, 0x40 };
  key_table_[fabgl::VK_LEFTPAREN] = { 9, 0x80 };
  key_table_[fabgl::VK_9] = { 9, 0x80 };
}

void Machine::load(int address, uint8_t const * data, int length)
{
  for (int i = 0; i < length; ++i) {
    m_RAM[address + i] = data[i];
  }

  m_RAM[0x1311] = 0x27; // 3b '(AT), :(SPC)
  m_RAM[0x1320] = 0x3d;
  m_RAM[0x1331] = 0x7c;
  m_RAM[0x1341] = 0x5d;
  m_RAM[0x1346] = 0x60;
  m_RAM[0x1350] = 0x7f;
  m_RAM[0x1351] = 0x29;
  m_RAM[0x1352] = 0x3a; // 22 :(AT), +(SPC)
  m_RAM[0x1355] = 0x28;
  m_RAM[0x1359] = 0x22; // 3a "(AT), *(SPC)
  m_RAM[0x135d] = 0x2a;
  m_RAM[0x1365] = 0x26;
  m_RAM[0x1368] = 0x2b;
  m_RAM[0x136d] = 0x5e; // 53
  m_RAM[0x1379] = 0x5c;
  m_RAM[0x138d] = 0x40;
  m_RAM[0x138e] = 0x7e;
}

void Machine::attachRAM(int RAMSize)
{

}


int Machine::nextStep()
{
  return m_Z80.step();
}

#define INTR_PERIOD 16666

void Machine::run()
{
  m_Z80.reset();

  int64_t instruction_timer = 0;
  int64_t interrupt_timer = INTR_PERIOD;
  int64_t cur_ts;

  constexpr int timeToCheckKeyboardReset = 200000;
  int timeToCheckKeyboard = timeToCheckKeyboardReset;


  Serial.printf("start:%lld\n", esp_timer_get_time());
  while (true) {
    // Using the cycles consumed by the instruction code, give a delay before
    // executing the next instruction. At 4MHz, each cycle lasts 0.25us, so
    // instruction time == cycles*0.25 == cycles/4
    int64_t cur_ts = esp_timer_get_time(); // us
    int cycles = nextStep();
    instruction_timer += cycles / 4;
    do {
      int64_t ts = esp_timer_get_time();
      int64_t time_past = ts - cur_ts;
      cur_ts = ts;
      instruction_timer -= time_past;
      interrupt_timer -= time_past;
      if (interrupt_timer < 0.f) {
        m_Z80.IRQ(/*not_used*/0);
        interrupt_timer += INTR_PERIOD;
      }
    } while (instruction_timer > 0.f);

    timeToCheckKeyboard -= cycles;
    if (timeToCheckKeyboard < 0) {
      timeToCheckKeyboard = timeToCheckKeyboardReset;
      auto kbd = fabgl::PS2Controller::keyboard();
      if (kbd->virtualKeyAvailable()) {
        VirtualKeyItem item;
        if (kbd->getNextVirtualKey(&item)) {
          KeyMat km = KeyMatFromVirt(item.vk);
          if (km.addr >= 0) {
            if (item.down)
              key_matrix_[km.addr] &= ~km.mask;
            else
              key_matrix_[km.addr] |= km.mask;
          }
        }
      }
    }
  }
  int count = readByte(this, 0x36);
  Serial.printf("IRQ-count: %d %d\n\r", count, readByte(this, 0x70));
  Serial.printf("end:%lld\n", esp_timer_get_time());
  while(true) {delay(1000);}
}


int Machine::readByte(void * context, int address) {
  return ((Machine*)context)->m_RAM[address];
}


void Machine::writeByte(void * context, int address, int value) {
  ((Machine*)context)->m_RAM[address] = value;
}


int Machine::readIO(void * context, int addr) {
  Machine *m = (Machine *)context;
  if (0x8000 <= addr && addr <= 0x8009) {
    return m->KeyIOMatrix(addr-0x8000);
  }
  return m->ReadVram(addr);
}


void Machine::writeIO(void * context, int addr, int value) {
  if ((addr & 0xe000) == 0) { // 0x0000~0x1fff
    Machine *m = (Machine *)context;
    m->WriteVram(addr, value);
  }
}

void Machine::WriteVram(int addr, int value) {
  vram_[addr] = value;
}

int Machine::ReadVram(int addr) {
  return vram_[addr];
}

KeyMat Machine::KeyMatFromVirt(fabgl::VirtualKey vk) {
  return key_table_[static_cast<int>(vk)];
}

int Machine::KeyIOMatrix(int index) {
  return key_matrix_[index];
}
