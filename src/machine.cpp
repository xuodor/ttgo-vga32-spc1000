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
const uint8_t Altair88DiskBootROM[256] = {
  0x21, 0x00, 0x4c, 0x11, 0x18, 0xff, 0x0e, 0xe6, 0x1a, 0x77, 0x13, 0x23,
  0x0d, 0xc2, 0x08, 0xff, 0xc3, 0x00, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xf3, 0x31, 0x62, 0x4d, 0xaf, 0xd3, 0x08, 0x3e, 0x04, 0xd3, 0x09, 0xc3,
  0x19, 0x4c, 0xdb, 0x08, 0xe6, 0x02, 0xc2, 0x0e, 0x4c, 0x3e, 0x02, 0xd3,
  0x09, 0xdb, 0x08, 0xe6, 0x40, 0xc2, 0x0e, 0x4c, 0x11, 0x00, 0x00, 0x06,
  0x00, 0xdb, 0x08, 0xe6, 0x04, 0xc2, 0x25, 0x4c, 0x3e, 0x10, 0xf5, 0xd5,
  0xc5, 0xd5, 0x11, 0x86, 0x80, 0x21, 0xd4, 0x4c, 0xdb, 0x09, 0x1f, 0xda,
  0x38, 0x4c, 0xe6, 0x1f, 0xb8, 0xc2, 0x38, 0x4c, 0xdb, 0x08, 0xb7, 0xfa,
  0x44, 0x4c, 0xdb, 0x0a, 0x77, 0x23, 0x1d, 0xca, 0x5a, 0x4c, 0x1d, 0xdb,
  0x0a, 0x77, 0x23, 0xc2, 0x44, 0x4c, 0xe1, 0x11, 0xd7, 0x4c, 0x01, 0x80,
  0x00, 0x1a, 0x77, 0xbe, 0xc2, 0xc1, 0x4c, 0x80, 0x47, 0x13, 0x23, 0x0d,
  0xc2, 0x61, 0x4c, 0x1a, 0xfe, 0xff, 0xc2, 0x78, 0x4c, 0x13, 0x1a, 0xb8,
  0xc1, 0xeb, 0xc2, 0xb5, 0x4c, 0xf1, 0xf1, 0x2a, 0xd5, 0x4c, 0xd5, 0x11,
  0x00, 0xff, 0xcd, 0xce, 0x4c, 0xd1, 0xda, 0xbe, 0x4c, 0xcd, 0xce, 0x4c,
  0xd2, 0xae, 0x4c, 0x04, 0x04, 0x78, 0xfe, 0x20, 0xda, 0x2c, 0x4c, 0x06,
  0x01, 0xca, 0x2c, 0x4c, 0xdb, 0x08, 0xe6, 0x02, 0xc2, 0xa0, 0x4c, 0x3e,
  0x01, 0xd3, 0x09, 0xc3, 0x23, 0x4c, 0x3e, 0x80, 0xd3, 0x08, 0xc3, 0x00,
  0x00, 0xd1, 0xf1, 0x3d, 0xc2, 0x2e, 0x4c, 0x3e, 0x43, 0x01, 0x3e, 0x4f,
  0x01, 0x3e, 0x4d, 0x47, 0x3e, 0x80, 0xd3, 0x08, 0x78, 0xd3, 0x01, 0xc3,
  0xc9, 0x4c, 0x7a, 0xbc, 0xc0, 0x7b, 0xbd, 0xc9, 0x84, 0x00, 0x4c, 0x24,
  0x16, 0x56, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};


////////////////////////////////////////////////////////////////////////////////////
// Machine


Machine::Machine()
  : m_devices(nullptr),
    m_realSpeed(false),
    m_menuCallback(nullptr)
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

  constexpr int timeToCheckKeyboardReset = 200000;
  int timeToCheckKeyboard = timeToCheckKeyboardReset;

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

    // time to check keyboard for menu key (F12 or PAUSE)?
    timeToCheckKeyboard -= cycles;
    if (timeToCheckKeyboard < 0) {
      timeToCheckKeyboard = timeToCheckKeyboardReset;
      auto keyboard = fabgl::PS2Controller::keyboard();
      if (m_menuCallback && (keyboard->isVKDown(VirtualKey::VK_PAUSE) || keyboard->isVKDown(VirtualKey::VK_F12)))
        m_menuCallback();
    }
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
  auto m = (Machine*)context;
  for (Device * d = m->m_devices; d; d = d->next) {
    int value;
    if (d->read(address, &value))
      return value;
  }

  // not handlded!

  //Serial.printf("readIO 0x%x\n\r", address);

  return 0xff;
}


void Machine::writeIO(void * context, int address, int value)
{
  auto m = (Machine*)context;
  for (Device * d = m->m_devices; d; d = d->next)
    if (d->write(address, value))
      return;

  // not handlded!

  //Serial.printf("writeIO 0x%x %d %c\n\r", address, value, (char)value);
}


// Machine
////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////
// SIO


SIO::SIO(Machine * machine, int address)
  : Device(machine),
    m_address(address),
    m_stream(nullptr)
{
  machine->attachDevice(this);
}


void SIO::attachStream(Stream * value)
{
  m_stream = value;
}


bool SIO::read(int address, int * result)
{
  if (address == m_address) {
    // CTRL
    bool available = false;
    if (m_stream)
      available = m_stream->available();
    *result = 0b10 | (available ? 1 : 0);
    return true;
  } else if (address == m_address + 1) {
    // DATA
    int ch = 0;
    if (m_stream && m_stream->available())
      ch = m_stream->read();
    *result = ch;
    return true;
  }
  return false;
}


bool SIO::write(int address, int value)
{
  if (address == m_address) {
    // CTRL
    return true;
  } else if (address == m_address + 1) {
    // DATA
    if (m_stream)
      m_stream->write(value);
    //fprintf(stderr, "SIO: %c\n", value);
    return true;
  }
  return false;
}

// SIO
////////////////////////////////////////////////////////////////////////////////////
