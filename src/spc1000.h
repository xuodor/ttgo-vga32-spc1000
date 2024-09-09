#pragma once

#include <stdint.h>
#include <stdio.h>

#include "fabgl.h"
#include "Z80.h"
#include "mc6847.h"
#include "ay38910.h"

#define CAS_STOP 0
#define CAS_PLAY 1
#define CAS_REC 2

typedef struct {
  int addr;
  uint8_t mask;
} KeyMat;

/**
 * Cassette structure for tape processing, included in the SPCIO
 */
typedef struct {
  int motor; // Motor Status
  int pulse; // Motor Pulse (0->1->0) causes motor state to flip
  int button;
  int rdVal;
  uint32_t startTime;
  uint32_t cnt0, cnt1;

  int wrVal;
  uint32_t wrRisingT; // rising time

  FILE *wfp;
  FILE *rfp;
  int dos;  // DOS command signal
} Cassette;

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

private:
  KeyMat KeyMatFromVirt(fabgl::VirtualKey vk);
  int KeyIOMatrix(int index) { return key_matrix_[index]; }
  void InitMem();

  uint8_t mem_[0x10000];
  uint8_t io_[0x2000];
  KeyMat key_table_[fabgl::VK_LAST];
  uint8_t key_matrix_[10];
  Cassette cas;
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
  SPCSimul simul;
};
