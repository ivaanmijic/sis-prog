#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int i = 0;

void ouch(int sig);

int main(int argc, char *argv[]) {
  signal(SIGINT, ouch);

  while (1) {
    i++;
    printf("Hello world!\n");
    sleep(1);
  }

  return EXIT_SUCCESS;
}

void ouch(int sig) {
  printf("OUCH! - I got signal %d\n", sig);
  /* exit(EXIT_SUCCESS); */
  signal(sig, SIG_DFL);
}
