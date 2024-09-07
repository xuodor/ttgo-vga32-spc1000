#include "ay38910.h"
#include "freertos/FreeRTOS.h"
#include "sndqueue.h"

AY38910::AY38910() {}

void AY38910::Init(fabgl::SoundGenerator *sg) {
  SndQueueInit();
  Reset();
  sg->setVolume(126);
  sg->play(true);
  sg->attach(this);
  enable(true);
  Loop(0);
}

int AY38910::getSample() {
  return SndGetSample();
}

void AY38910::Reset() {
  Reset8910(&psg_, AY8910_BASE << 4, 0);
}

void AY38910::Write(byte data) {
  Write8910(&psg_, reg_, data);
}

void AY38910::WrCtrl(byte data) {
  reg_ = data;
  WrCtrl8910(&psg_, data);
}

byte AY38910::RdData() {
  return RdData8910(&psg_);
}

void AY38910::Loop(int mS) {
  Loop8910(&psg_, mS);
}
