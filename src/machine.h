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


#pragma once


#include <stdint.h>
#include <stdio.h>

#include "fabutils.h"

#include "emudevs/Z80.h"


extern const uint8_t Altair88DiskBootROM[8];
const int Altair88DiskBootROMAddr = 0x0000;
const int Altair88DiskBootROMRun  = 0x0000;


class Machine;



////////////////////////////////////////////////////////////////////////////////////
// Device


struct Device {
  Device(Machine * machine) : m_machine(machine) { }

  Device * next = nullptr;
  virtual bool read(int address, int * result) = 0;   // ret true = handled
  virtual bool write(int address, int value) = 0;     // ret true = handled

  Machine * machine() { return m_machine; }

  virtual void tick(int ticks) = 0;

private:
  Machine * m_machine;
};



////////////////////////////////////////////////////////////////////////////////////
// Machine


typedef void (*MenuCallback)();


class Machine {

public:

  Machine();

  ~Machine();

  void load(int address, uint8_t const * data, int length);

  void attachDevice(Device * device);

  void attachRAM(int RAMSize);

  void run(int address);

  static int readByte(void * context, int address);
  static void writeByte(void * context, int address, int value);

  static int readWord(void * context, int addr)              { return readByte(context, addr) | (readByte(context, addr + 1) << 8); }
  static void writeWord(void * context, int addr, int value) { writeByte(context, addr, value & 0xFF); writeByte(context, addr + 1, value >> 8); }

  static int readIO(void * context, int address);
  static void writeIO(void * context, int address, int value);

  void setRealSpeed(bool value) { m_realSpeed = value; }
  bool realSpeed() { return m_realSpeed; }

private:

  int nextStep();

  Device *     m_devices;
  bool         m_realSpeed;
  uint8_t *    m_RAM;
  fabgl::Z80   m_Z80;
};
