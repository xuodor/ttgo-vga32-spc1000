/** EMULib Emulation Library *********************************/
/**                                                         **/
/**                          Sound.c                        **/
/**                                                         **/
/** This file file implements core part of the sound API    **/
/** and functions needed to log soundtrack into a MIDI      **/
/** file. See Sound.h for declarations.                     **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996-2008                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

/*************************************************************/
/** Ported to run on ESP32 running FreeRTOS sound queue.    **/
/** Sound queue is for precise, delayed sound processing.   **/
/**                             ionique, 2006               **/
/**                             jindor, 2024                **/
/*************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "sndqueue.h"

int LastBufTime = 0;
QueueHandle_t queue_;

int GetTicks() {
  return esp_timer_get_time() / 1000;  // ms
}

static int Freq[6] = {0};
static int Vol[6];
static int Interval[6];
static int NoiseInterval[6];
static int Phase[6];
static int DevFreq;

#define DEVFREQ 32900
//#define DEVFREQ 44100

void SndLatch(int Chn, int Freq, int Volume) {
  int interval;

  interval = Freq ? (DevFreq / Freq) : 0;
  if (interval != Interval[Chn]) {
    Interval[Chn] = interval;
    Phase[Chn] = 0;
  }
  if (Interval[Chn] == 0)
    Vol[Chn] = 0;
  else
    Vol[Chn] = Volume * 30;
}

void SndQueueInit(void) {
  DevFreq = DEVFREQ;
  queue_ = xQueueCreate(100, sizeof(TSndQEntry));
  LastBufTime = GetTicks();
}

int Sound(int Chn, int Freq, int Vol) {
  TSndQEntry entry;
  entry.chn = Chn;
  entry.freq = Freq;
  entry.vol = Vol;
  entry.time = GetTicks() - LastBufTime;
  if (errQUEUE_FULL == xQueueSend(queue_, &entry, 1)) {
    // Error
    return 0;
  }
  return 1;
}

int SndDequeue(TSndQEntry *entry) {
   return xQueueReceiveFromISR(queue_, entry, NULL);
}

int SndPeekQueue() {
  TSndQEntry entry;
  return pdFALSE != xQueuePeekFromISR(queue_, &entry);
}

int SndGetSample() {
  int R1 = 0;
  int i;
  int vTime, qTime; // virtual Time, queue Time
  TSndQEntry qentry;

  while (SndPeekQueue()) {
    SndDequeue(&qentry);
    SndLatch(qentry.chn, qentry.freq, qentry.vol);
  }

  R1 = 0;
  // Tone Generation
  for (i = 0; i < 3; i++) {
    if (Interval[i] && Vol[i]) {
      if (Phase[i] < (Interval[i] / 2)) {
        R1 += Vol[i];
        R1 = (R1 > 32767) ? 32767 : R1;
      } else if (Phase[i] >= (Interval[i] / 2) && Phase[i] < Interval[i]) {
        R1 -= Vol[i];
        R1 = (R1 < -32768) ? -32768 : R1;
      }
      Phase[i]++;
      if (Phase[i] >= Interval[i]) {
        Phase[i] = 0;
        // R1 += Vol[i];
      }
    }
  }

  // Noise Generation
  for (i = 3; i < 6; i++) {
    if (Interval[i] && Vol[i]) {
      if (Phase[i] == 0)
        NoiseInterval[i] = Interval[i] + (((150 * rand()) / RAND_MAX) - 75);
      if (Phase[i] < (NoiseInterval[i] / 2)) {
        R1 += Vol[i];
        R1 = (R1 > 32767) ? 32767 : R1;
      } else if (Phase[i] >= (NoiseInterval[i] / 2) &&
                 Phase[i] < NoiseInterval[i]) {
        R1 -= Vol[i];
        R1 = (R1 < -32768) ? -32768 : R1;
      }
      Phase[i]++;
      if (Phase[i] >= NoiseInterval[i]) {
        Phase[i] = 0;
        // R1 += Vol[i];
      }
    }
  }

  LastBufTime = GetTicks();
  while (SndPeekQueue()) {
    SndDequeue(&qentry);
    SndLatch(qentry.chn, qentry.freq, qentry.vol);
  }
  return R1;
}

void SetSound(int Channel, int NewType) {
}

void Drum(int Type,int Force) {
}
