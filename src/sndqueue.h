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

#define MAX_SNDQ 100

//#define AY8910_BASE     111861 /* Base frequency for AY8910  */
#define AY8910_BASE     223722 /* Base frequency for AY8910  */

#ifdef __cplusplus
extern "C" {
#endif

void SndQueueInit();
int SndGetSample();

#ifdef __cplusplus
}
#endif
