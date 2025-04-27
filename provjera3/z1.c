#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_term;
int ttyfd = STDIN_FILENO;

void fatal(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

void tty_on_exit() { tcsetattr(ttyfd, TCSAFLUSH, &orig_term); }

void set_tty() {
  struct termios raw;
  if (tcgetattr(ttyfd, &raw) < 0)
    fatal("tcgetattr error");

  raw.c_lflag = ~(ECHO | ECHONL | ICANON | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 10;

  if (tcsetattr(ttyfd, TCSAFLUSH, &raw) < 0)
    fatal("tcsetattr error");
}

void process_input() {
  int nread;
  char c_in, c_out;
  while (1) {
    nread = read(ttyfd, &c_in, 1);
    if (nread < 0)
      fatal("read error");
    if (c_in == 'q')
      exit(EXIT_SUCCESS);
    if (c_in == 'w')
      write(STDOUT_FILENO, "unos", 5);
  }
}

int main(void) {
  if (!isatty(ttyfd))
    fatal("nije terminal");

  if (tcgetattr(ttyfd, &orig_term) < 0)
    fatal("tcgetattr error");

  atexit(tty_on_exit);

  set_tty();
  process_input();

  return EXIT_SUCCESS;
}
