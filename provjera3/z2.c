#include <bits/types/siginfo_t.h>
#include <bits/types/sigset_t.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void alarm_handler(int sig, siginfo_t *info, void *ucontext) {
  printf("Pid: %d, signal: %d\n", info->si_pid, info->si_signo);
  pause();
}

int main(int argc, char *argv[]) {
  struct sigaction sa;
  sa.sa_sigaction = alarm_handler;
  sa.sa_flags = 0;

  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGINT);
  sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL);

  sigaction(SIGALRM, &sa, NULL);

  kill(getpid(), SIGINT);
  sigset_t pending_signals;
  sigpending(&pending_signals);
  if (sigismember(&pending_signals, SIGINT))
    printf("Interupt je pending\n");
  else
    printf("Interupt nije pending\n");

  kill(getpid(), SIGALRM);

  return EXIT_SUCCESS;
}
