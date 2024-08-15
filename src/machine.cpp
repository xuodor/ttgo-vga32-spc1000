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







// Altair 88-DSK Boot ROM (starts at 0xFF00)
const uint8_t Altair88DiskBootROM[8] = { 0x01, 0x02, 0xa0, 0x3e, 0x55, 0xed, 0x79, 0x76 };


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
  delete [] m_RAM;
}


void Machine::attachDevice(Device * device)
{
  device->next = m_devices;
  m_devices = device;
}


void Machine::load(int address, uint8_t const * data, int length)
{
  for (int i = 0; i < length; ++i)
    m_RAM[address + i] = data[i];
}


void Machine::attachRAM(int RAMSize)
{
  m_RAM = new uint8_t[RAMSize];
}


int Machine::nextStep()
{
  return m_Z80.step();
}


void Machine::run(int address)
{
  m_Z80.reset();

  while (true) {
    int cycles = 0;
    if (m_realSpeed) {
      int64_t t = esp_timer_get_time();  // time in microseconds
      cycles = nextStep();
      if (m_realSpeed) {
        t += cycles / 2;                 // at 2MHz each cycle last 0.5us, so instruction time is cycles*0.5, that is cycles/2
        while (esp_timer_get_time() < t)
          ;
      }
    } else {
      cycles = nextStep();
    }
    for (Device * d = m_devices; d; d = d->next)
      d->tick(cycles);
  }

}


int Machine::readByte(void * context, int address)
{
  return ((Machine*)context)->m_RAM[address];
}


void Machine::writeByte(void * context, int address, int value)
{
  ((Machine*)context)->m_RAM[address] = value;
}


int Machine::readIO(void * context, int address)
{
  Serial.printf("readIO 0x%x\n\r", address);
  auto m = (Machine*)context;
  for (Device * d = m->m_devices; d; d = d->next) {
    int value;
    if (d->read(address, &value))
      return value;
  }

  // not handlded!


  return 0xff;
}


void Machine::writeIO(void * context, int address, int value)
{
  Serial.printf("writeIO 0x%x %d %c\n\r", address, value, (char)value);
  auto m = (Machine*)context;
  for (Device * d = m->m_devices; d; d = d->next)
    if (d->write(address, value))
      return;

  // not handlded!

}
