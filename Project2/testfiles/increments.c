/*
Author: Eric Kaneshige
Email: etkanesh@ucsc.edu
CREATED: 5/2/2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>
#include <time.h>

extern char **get_line(void);
/*define sig used by bg process*/

/*typedef void (*sighandler_t)(int);*/
sighandler_t mysig;
int flag = 0;

void cure(int sig)
{
  pid_t pid;
  if(flag == 0) return;
  while ((pid = waitpid(-1, NULL, WNOHANG)) > 0){
    flag--;
  }
  if(flag == 0) signal(SIGCHLD, mysig);
}

int main(int argc, char *argv[])
{
  /* parameter 0: initial value
  increments by 0.00001
  parameter 1: target max number, if a max is desired */
  int counter; /* for print */
  int status;
  double current;
  double max;
  char **args;
  int i;
  int size;
  pid_t pid;
  int threadnum;
  int runs;
  int numprocesses;
  clock_t t1, t2;
  double time;
  threadnum = 0;
  counter = 0;
  if (argc > 1) {
    numprocesses = atoi(argv[1]);
  } else {
    numprocesses = 1;
  }
  printf("%d\n", numprocesses);
  runs = 0;
  while (runs < numprocesses) {
    t1 = clock();
    runs++;
    threadnum++;
    flag = 0;
    current = threadnum * 2;
    max = current + (100 * 1);


    for(i = 0; i < 1; i++) {
      if (runs < numprocesses) {
    flag++;
    mysig = signal(SIGCHLD, cure);
    break;
      }
    }
    pid = fork();
    if (pid == -1) {
      printf("Failed fork.\n");
    } else if (pid != 0) {
      if (flag == 0) {
        while(wait(&status) != pid) {}
      }

    } else {
      while(current <= max) {
        counter++;
        current += 0.000001;
      }
      t2 = clock();
      time = ((double)(t2 -t1) * 1000)/1000000000*10000;
      printf("Done with Fork #%d at time %f seconds!\n", threadnum, time);
      exit(1);
      break;
    }
  }

  return 0;
}
