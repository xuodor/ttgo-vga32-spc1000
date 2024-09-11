#include "cassette.h"

extern uint32_t cas_start_time();

/**
 * Reset Cassette structure
 * @param cas cassette structure
 */
void ResetCassette(Cassette *cas) {
  cas->rdVal = -2;
  cas->cnt0 = cas->cnt1 = 0;

  cas->wrVal = 0; // correct location?
  cas->wrRisingT = 0;
}

/**
 * Initialize cassette structure
 * @param cas cassette structure
 */
void InitCassette(Cassette *cas) {
  cas->button = CAS_STOP;

  cas->wfp = NULL;
  cas->rfp = NULL;
  ResetCassette(cas);
}

/*************************************************************/
/** Cassette Tape Processing                                **/
/*************************************************************/
int ReadVal(Cassette *cas) {
  int c = -1;

  if (cas->rfp != NULL) {
    static int EOF_flag = 0;

    c = fgetc(cas->rfp);
    if (c == EOF) {
      if (!EOF_flag) {
        EOF_flag = 1;
      }
      c = -1;
    } else {
      EOF_flag = 0;
      c -= '0';
    }
  }
  return c;
}

int CasRead(Cassette *cas) {
  uint32_t curTime;
  int bitTime;

  curTime = cas_start_time() - cas->startTime;
  bitTime = (cas->rdVal == 0) ? 56 : (cas->rdVal == 1) ? 112 : 0;

  while (curTime >= (cas->cnt0 * 56 + cas->cnt1 * 112 + bitTime)) {
    if (cas->rdVal == 0)
      cas->cnt0++;
    if (cas->rdVal == 1)
      cas->cnt1++;

    cas->rdVal = ReadVal(cas);

    if (cas->rdVal == -1)
      break;

    bitTime = (cas->rdVal == 0) ? 56 : (cas->rdVal == 1) ? 112 : 0;
  }

  curTime -= (cas->cnt0 * 56 + cas->cnt1 * 112);

  switch (cas->rdVal) {
  case 0:
    if (curTime < 28)
      return 1; // high
    else
      return 0; // low

  case 1:
    if (curTime < 56)
      return 1; // high
    else
      return 0; // low
  }
  return 0; // low for other cases
}

void CasWrite(Cassette *cas, int val) {
  uint32_t curTime;

  curTime = cas_start_time();
  if (!cas->wrVal & val) // rising edge
    cas->wrRisingT = curTime;
  if (cas->wrVal & !val) { // falling edge
    byte b = (curTime - cas->wrRisingT) < 32 ? '0' : '1';
    if (cas->wfp != NULL) {
      fputc(b, cas->wfp);
    }
  }

  cas->wrVal = val;
}

void CasIOWrite(Cassette *cas, byte Value) {
    if (cas->button != CAS_STOP) {
      if (Value & 0x02) { // Motor
        if (cas->pulse == 0) {
          cas->pulse = 1;
        }
      } else {
        if (cas->pulse) {
          cas->pulse = 0;
          if (cas->motor) {
            cas->motor = 0;
          } else {
            cas->motor = 1;
            cas->startTime = cas_start_time();
            ResetCassette(cas);
          }
        }
      }
    } // != CAS_STOP

    if (cas->button == CAS_REC && cas->motor)
      CasWrite(cas, Value & 0x01);

}

byte CasIORead(Cassette *cas) {
  byte retval = 0x1f;
  // 0x80 - cassette data input
  // 0x40 - motor status
  if (cas->button == CAS_PLAY && cas->motor) {
    if (CasRead(cas) == 1)
      retval |= 0x80; // high
    else
      retval &= 0x7f; // low
  }
  if (cas->motor)
    return (retval & (~(0x40))); // 0 indicates Motor On
  else
    return (retval | 0x40);
}
