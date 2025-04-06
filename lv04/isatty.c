#include "unistd.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (isatty(1))
    printf("Jest");
  else
    printf("Nije");
  return 0;
}
