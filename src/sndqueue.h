
/*************************************************************/
/** Sound queue processing                                  **/
/*************************************************************/

/**
 * Sound Queue entry
 */
typedef struct {
  int chn;
  int freq;
  int vol;
  int time; // in ms
} TSndQEntry;

#define MAX_SNDQ 1000 // 1000 is enough for most cases

/**
 * Sound Queue structure
 */
typedef struct {
  int front;
  int rear;
  TSndQEntry qentry[MAX_SNDQ];
} TSndQ;

//#define AY8910_BASE     111861 /* Base frequency for AY8910  */
#define AY8910_BASE     223722 /* Base frequency for AY8910  */

#define SND_MELODIC     0      /* Melodic sound (default)    */
#define SND_RECTANGLE   0      /* Rectangular wave           */
#define SND_TRIANGLE    1      /* Triangular wave (1/2 rect.)*/
#define SND_NOISE       2      /* White noise                */
#define SND_PERIODIC    3      /* Periodic noise (not im-ed) */
#define SND_WAVE        4      /* Wave sound set by SetWave()*/
#define SND_MIDI        0x100  /* MIDI instrument (ORable)   */

#ifdef __cplusplus
extern "C" {
#endif

void SndQueueInit();
int SndEnqueue(int Channel,int Freq,int Volume);
int SndDequeue(TSndQEntry *entry);
int SndGetSample();

void Drum(int Type,int Force);

#ifdef __cplusplus
}
#endif
