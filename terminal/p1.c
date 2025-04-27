#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (isatty(1))
    printf("%s\n", ttyname(1));
  else if (errno == EBADF)
    printf("nije fd");
  else
    printf("nije terminal!\n");
  return EXIT_SUCCESS;
}
