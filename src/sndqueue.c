/*************************************************************/
/** Libsdl sound driver + sound queue processing            **/
/** Sound queue is for precise, delayed sound processing.   **/
/**                             ionique, 2006               **/
/*************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "sndqueue.h"

int LastBufTime = 0;
QueueHandle_t queue_;

int GetTicks() {
  return esp_timer_get_time() / 1000;  // ms
}

static int Freq[6];
static int Vol[6];
static int Interval[6];
static int NoiseInterval[6];
static int Phase[6];
static int DevFreq;
static volatile int JF = 0;

//#define DEVFREQ 22050
#define DEVFREQ 44100

void Sound(int Chn, int Freq, int Volume) {
  int interval;

  interval = Freq ? (DevFreq / Freq) : 0;
  if (interval != Interval[Chn]) {
    Interval[Chn] = interval;
    Phase[Chn] = 0;
  }
  if (Interval[Chn] == 0)
    Vol[Chn] = 0;
  else
    Vol[Chn] = Volume * 5;
}

void SndQueueInit(void) {
  DevFreq = DEVFREQ;
  queue_ = xQueueCreate(100, sizeof(TSndQEntry));

  LastBufTime = GetTicks();
  for (int i = 0; i < 6; i++)
    Sound(i, 0, 0);
}

int SndEnqueue(int Chn, int Freq, int Vol) {
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

  if (pdFALSE == xQueuePeekFromISR(queue_, &entry)) {
    return -1; // likely empty
  } else {
    return entry.time;
  }
}

int SndGetSample() {
  int R1 = 0;
  int i;
  int vTime, qTime; // virtual Time, queue Time
  TSndQEntry qentry;

  vTime = 0;
  qTime = SndPeekQueue();

  while ((qTime != -1) && (vTime - qTime) > 0) // Check Sound Queue
  {
    SndDequeue(&qentry);

    Sound(qentry.chn, qentry.freq, qentry.vol);
    qTime = SndPeekQueue();
  }

  R1 = 0;
  for (i = 0; i < 3; i++) // Tone Generation
  {
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

  for (i = 3; i < 6; i++) // Noise Generation
  {
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
  while (SndPeekQueue() >= 0) {
    SndDequeue(&qentry);
    Sound(qentry.chn, qentry.freq, qentry.vol);
  }
  return R1;
}

void SetSound(int Channel, int NewType) {
  //	printf("SetSound: Chn=%d, NewType=%d\n", Channel, NewType);
}

void Drum(int Type,int Force) {

}
