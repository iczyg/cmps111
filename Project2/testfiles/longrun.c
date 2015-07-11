#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define	LOOP_COUNT_MIN	100
#define	LOOP_COUNT_MAX	100000000
int
main (int argc, char *argv[])
{
  char *idStr;
  unsigned int	v;
  int	i = 0;
  int	iteration = 1;
  int	loopCount;
  int	maxloops;

  if (argc < 3 || argc > 4) {
    printf ("Usage: %s [some value] [loops] [stop number]\n", argv[0]);
    exit (-1);
  }
  v = getpid ();
  idStr = argv[1];
  loopCount = atoi (argv[2]);
  if ((loopCount < LOOP_COUNT_MIN) || (loopCount > LOOP_COUNT_MAX)) {
    exit (-1);
  }
  if (argc == 4) {
    maxloops = atoi (argv[3]);
  } else {
    maxloops = 0;
  }

 
  while (1) {
    v = (v << 4) - v;
    if (++i == loopCount) {
      if (iteration == maxloops) {
	break;
      }
      printf ("%s:%06d\n", idStr, iteration);
      /*fflush (stdout);*/
      iteration += 1;
      i = 0;
    }
  }
  printf ("The final value of v is 0x%08x\n", v);
}
