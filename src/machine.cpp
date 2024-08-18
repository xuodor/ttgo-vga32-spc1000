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
  load(0, spcrom, 0x80);

  mc6847.begin();
  mc6847.setResolution(VGA_640x480_60Hz);

  mc6847.setPaletteItem(0, RGB888(0, 0, 0));
  mc6847.setPaletteItem(1, RGB888(255, 0, 0));
  mc6847.setPaletteItem(2, RGB888(0, 255, 0));
  mc6847.setPaletteItem(3, RGB888(0, 0, 255));
  mc6847.setPaletteItem(4, RGB888(255, 255, 0));
  mc6847.setPaletteItem(5, RGB888(255, 0, 255));
  mc6847.setPaletteItem(6, RGB888(0, 255, 255));
  mc6847.setPaletteItem(7, RGB888(255, 255, 255));

  for (int y = 0; y < 480; ++y) {
    for (int x = 0; x < 640; ++x) {
      if (!(48 <= y && y < 480-64 && 64 <= x && x < 640-64)) {
        mc6847.directSetPixel(x, y, 2);
      }
    }
  }

}

void Machine::load(int address, uint8_t const * data, int length)
{
  for (int i = 0; i < length; ++i)
    m_RAM[address + i] = data[i];
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
  const int refresh_set_ = 0;

  m_Z80.reset();

  int64_t instruction_timer = 0;
  int64_t interrupt_timer = INTR_PERIOD;
  int64_t cur_ts;
  int refresh_timer = refresh_set_;

  Serial.printf("start:%lld\n", esp_timer_get_time());
  for (int i = 0; i < 100000; ++i) {
    // Using the cycles consumed by the instruction code, give a delay before
    // executing the next instruction. At 4MHz, each cycle lasts 0.25us, so
    // instruction time == cycles*0.25 == cycles/4
    int64_t cur_ts = esp_timer_get_time(); // us
    int cycles = nextStep();
    instruction_timer += cycles / 4;
//    Serial.printf("cycles:%d pc:%x\n", cycles, m_Z80.getPC());
    do {
      int64_t ts = esp_timer_get_time();
      int64_t time_past = ts - cur_ts;
      cur_ts = ts;
//      Serial.printf("diff: %lld code:%lld interrupt:%lld\n", time_past, instruction_timer, interrupt_timer);
      instruction_timer -= time_past;
      interrupt_timer -= time_past;
      if (interrupt_timer < 0.f) {
//        Serial.printf("IRQ\n");
        m_Z80.IRQ(/*not_used*/0);
        interrupt_timer += INTR_PERIOD;
        if (refresh_timer <= 0) {
          // Refresh screen 60Hz by default, same rate as the interrupt.
          refresh_timer = refresh_set_;
        }
        if (refresh_set_) refresh_timer--;
      }
    } while (instruction_timer > 0.f);
  }
  int count = readByte(this, 0x36);
  Serial.printf("IRQ-count: %d %d\n\r", count, readByte(this, 0x70));
  Serial.printf("end:%lld\n", esp_timer_get_time());

  while(true) {delay(1000);}
}


int Machine::readByte(void * context, int address)
{
  return ((Machine*)context)->m_RAM[address];
}


void Machine::writeByte(void * context, int address, int value)
{
  ((Machine*)context)->m_RAM[address] = value;
}


int Machine::readIO(void * context, int addr)
{
  Serial.printf("readIO 0x%x\n\r", addr);
  return 0xff;
}


void Machine::writeIO(void * context, int addr, int value) {
  if ((addr & 0xe000) == 0) { // 0x0000~0x1fff
//    Machine *m = (Machine *)context;
//    m->WriteVram(addr, value);
  }
}

void Machine::WriteVram(int addr, int value) {
  vram_[addr] = value;
}
