#pragma once

#include <stdint.h>
#include <stdio.h>

#include "fabgl.h"
#include "mc6847.h"
#include "ay38910.h"
#include "Z80.h"

typedef struct {
  int addr;
  uint8_t mask;
} KeyMat;

/**
 * Timestamps to keep emulation speed at 4MHz
 */
typedef struct {
  int32_t baseTick, curTick, prevTick;
} SPCSimul;

class SPC1000 {
public:
  SPC1000();
  ~SPC1000();

  void Init();
  void Run();
  MC6847 *vdg() { return &mc6847_; }
  void WriteMem(int addr, int value) { mem_[addr] = value; }
  int ReadMem(int addr);
  void WriteIO(int addr, int value);
  int ReadIO(int addr);
  uint32_t cas_start_time() { return tick*125 + ((4000-cpu_.ICount)>>5); }

private:
  KeyMat KeyMatFromVirt(fabgl::VirtualKey vk);
  int KeyIOMatrix(int index) { return key_matrix_[index]; }
  void InitMem();
  void PollKeyboard();

  uint8_t mem_[0x10000];
  uint8_t io_[0x2000];
  KeyMat key_table_[fabgl::VK_LAST];
  uint8_t key_matrix_[10];
  fabgl::PS2Controller keyboard_;
  fabgl::SoundGenerator sound_generator_;

  Z80 cpu_;
  MC6847 mc6847_;
  AY38910 ay38910_;

  uint8_t gmode;
  int iplk_;

  int32_t tick;
  int turbo;
  int refrTimer;   // timer for screen refresh
  int refrSet;     // init value for screen refresh timer
  double intrTime; // variable for interrupt timing
  int32_t kbd_timer;
  SPCSimul simul;
};
