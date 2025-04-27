#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;
int ttyfd = STDIN_FILENO;

int tty_reset() {
  if (tcsetattr(ttyfd, TCSAFLUSH, &orig_termios) < 0)
    return -1;
  return 0;
}

void tty_atexit() { tty_reset(); }

void fatal(char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

void set_tty() {
  struct termios raw;
  raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 10;
  if (tcsetattr(ttyfd, TCSAFLUSH, &raw) < 0)
    fatal("cannot set raw mode");
}

int process_term() {
  int n;
  char c_in, c_out, up[] = "\033[A", down[] = "\033[B";
  while (1) {
    n = read(ttyfd, &c_in, 1);
    if (n < 0)
      fatal("read error");
    else
      switch (c_in) {
      case 'q':
        return 0;
      case 'z':
        write(STDOUT_FILENO, "Z", 1);
        break;
      case 'u':
        write(STDOUT_FILENO, up, 3);
        break;
      case 'd':
        write(STDOUT_FILENO, down, 3);
        break;
      case 'c':
        write(STDOUT_FILENO, "\e[1;34m", 7);
        break;
      default:
        c_out = '*';
        write(STDOUT_FILENO, &c_out, 1);
      }
  }
}

int main(int argc, char *argv[]) {
  if (!isatty(ttyfd))
    fatal("not a tty");

  if (tcgetattr(ttyfd, &orig_termios) < 0)
    fatal("cannot get tty settings");

  if (atexit(tty_atexit) != 0)
    fatal("atexit: cannot regiseter tty reset");

  set_tty();
  process_term();

  return EXIT_SUCCESS;
}
