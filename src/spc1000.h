#pragma once

#include <stdint.h>
#include <stdio.h>

#include "fabgl.h"
#include "mc6847.h"
#include "ay38910.h"
#include "z80/Z80.h"
#include "cassette.h"
#include "dos.h"

typedef struct {
  int addr;
  byte mask;
} KeyMat;


/**
 * Timestamps to keep emulation speed at 4MHz
 */
typedef struct {
  int32_t base, cur, prev;
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
  uint32_t cas_start_time() { return tick_*125 + ((4000-cpu_.ICount)>>5); }

private:
  int KeyIOMatrix(int index) { return key_matrix_[index]; }
  void InitMem();
  void PollKeyboard();
  void ProcessEmulatorKey(VirtualKeyItem *item);

  byte *mem_;
  byte *io_;
  KeyMat *key_table_;
  byte key_matrix_[10];
  Cassette cas_;
  fabgl::PS2Controller keyboard_;
  fabgl::SoundGenerator sound_generator_;

  Z80 cpu_;
  MC6847 mc6847_;
  AY38910 ay38910_;

  byte gmode_;
  int iplk_;

  int32_t tick_;
  int turbo_;
  int32_t kbd_timer_;
  SPCSimul simul_;
};
