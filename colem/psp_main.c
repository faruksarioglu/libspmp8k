/*
   PSPColem: Porting of Colem
   Ludovic Jacomme <Ludovic.Jacomme@gmail.com>
*/

#include <stdio.h>
#include <zlib.h>
#include "global.h"
#include "SDL.h"

#include "Coleco.h"

#include <stdlib.h>
#include <stdio.h>
#include "cpulcd.h"
#include "gp2x_psp.h"

#define STDOUT_FILE  "stdout.txt"
#define STDERR_FILE  "stderr.txt"

extern int SDL_main(int argc, char *argv[]);

static void cleanup_output(void);

/* Remove the output files if there was no output written */
static void cleanup_output(void)
{
#ifndef NO_STDIO_REDIRECT
  FILE *file;
  int empty;
#endif

  /* Flush the output in case anything is queued */
  fclose(stdout);
  fclose(stderr);

#ifndef NO_STDIO_REDIRECT
  /* See if the files have any output in them */
  file = fopen(STDOUT_FILE, "rb");
  if ( file ) {
    empty = (fgetc(file) == EOF) ? 1 : 0;
    fclose(file);
    if ( empty ) {
      remove(STDOUT_FILE);
    }
  }
  file = fopen(STDERR_FILE, "rb");
  if ( file ) {
    empty = (fgetc(file) == EOF) ? 1 : 0;
    fclose(file);
    if ( empty ) {
      remove(STDERR_FILE);
    }
  }
#endif
}

int 
main(int argc, char *argv[])
{

  SDL_main(argc,argv);

  return 0;
}
