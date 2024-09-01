#pragma once

#include <stdint.h>
#include <stdio.h>

#include "fabgl.h"
#include "emudevs/Z80.h"
#include "mc6847.h"

typedef struct {
  int addr;
  uint8_t mask;
} KeyMat;


class SPC1000 {
public:
  SPC1000();
  ~SPC1000();

  void Init();
  void Run();
  MC6847 *vdg() { return &mc6847_; }
  void WriteMem(int addr, int value) { mem_[addr] = value; }
  int ReadMem(int addr) { return mem_[addr]; }
  void WriteIO(int addr, int value);
  int ReadIO(int addr);

private:
  KeyMat KeyMatFromVirt(fabgl::VirtualKey vk);
  int KeyIOMatrix(int index) { return key_matrix_[index]; }
  void LoadMem(int address, uint8_t const * data, int length);

  uint8_t mem_[0x10000];
  uint8_t io_[0x2000];
  fabgl::Z80 cpu_;
  MC6847 mc6847_;
  fabgl::PS2Controller keyboard_;
  KeyMat key_table_[fabgl::VK_LAST];
  uint8_t key_matrix_[10];
};
