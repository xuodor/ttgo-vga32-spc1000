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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "fabgl.h"
#include "fabutils.h"

#include "emudevs/Z80.h"
#include "mc6847.h"

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

typedef struct {
  int addr;
  uint8_t mask;
} KeyMat;


class Machine {

public:

  Machine();

  ~Machine();

  void init();
  void load(int address, uint8_t const * data, int length);

  void attachDevice(Device * device);

  void attachRAM(int RAMSize);

  MC6847 *vdg() { return &mc6847; }
  void run();
  void WriteVram(int addr, int value);
  int ReadVram(int addr);

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
  KeyMat KeyMatFromVirt(fabgl::VirtualKey vk);
  int KeyIOMatrix(int index);

  TaskHandle_t task_handle_;
  Device *     m_devices;
  bool         m_realSpeed;
  uint8_t m_RAM[0x10000];
  uint8_t vram_[0x2000];
  fabgl::Z80   m_Z80;
  MC6847 mc6847;
  fabgl::PS2Controller keyboard_;
  KeyMat key_table_[fabgl::VK_LAST];
  uint8_t key_matrix_[10] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
};
