#pragma once

#include <stdlib.h>

typedef struct {
     size_t   gl_pathc;    /* Count of paths matched so far  */
     char   **gl_pathv;    /* List of matched pathnames.  */
     size_t   gl_offs;     /* Slots to reserve in gl_pathv.  */
} glob_t;

#ifdef __cplusplus
extern "C" {
#endif

int glob(const char *pattern, int flags,
         int (*errfunc) (const char *epath, int eerrno),
         glob_t *pglob);
void globfree(glob_t *pglob);

#ifdef __cplusplus
}
#endif
