#include "glob.h"
//#include "freertos/FreeRTOS.h"
#include "sysdep.h"
#include <fnmatch.h>

int glob(const char *pattern, int flags,
         int (*errfunc) (const char *epath, int eerrno),
         glob_t *pglob) {
  int c = fs()->count();
  pglob->gl_pathv = (char **)malloc(sizeof(char*) * c);
  int tapfiles = 0;

  for (int i = 0; i < c; ++i) {
    char const *name = fs()->get(i)->name;
    size_t l = strlen(name);
    if (!fnmatch(pattern, name, 0)) {
        pglob->gl_pathv[tapfiles++] = (char *)name;
    }
  }
  pglob->gl_pathc = tapfiles;
  pglob->gl_offs = 0;
  return 0;
}

void globfree(glob_t *pglob) {
  free(pglob->gl_pathv);
}
