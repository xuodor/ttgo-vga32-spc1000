#pragma once

#include "devdrivers/soundgen.h"
#include "AY8910.h"

class AY38910 : public fabgl::WaveformGenerator {
 public:
  AY38910();

  void Init(fabgl::SoundGenerator *sg);

  // WaveformGenerator implementation
  void setFrequency(int value) {}
  int getSample();

  // Interface to Marat's AY8910 emulator
  void Reset();
  void Write(byte data);
  void WrCtrl(byte data);
  void WrData(byte data) {}  // Not used
  byte RdData();
  void Loop(int ms);

  byte reg() { return reg_; }
 private:
  AY8910 psg_;
  byte reg_;
};
