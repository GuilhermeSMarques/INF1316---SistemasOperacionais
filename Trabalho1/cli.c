#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: ./cli <PID_do_KernelSim>\n");
    exit(1);
  }
  int kernel_pid = atoi(argv[1]);
  printf("----COMMAND LINE INTERFACE FOR KERNEL %d -----\n", kernel_pid);
  printf(" - Press 'p' to pause\n");
  printf(" - Press 'c' to resume\n");
  printf("----------------------------------------------\n");
  char command = ' ';
  while (command != 'e') {
    int scan_result = scanf("%c", &command);
    if (command == 'p') {
      kill(kernel_pid, SIGTERM);
    }
    if (command == 'c') {
      kill(kernel_pid, SIGABRT);
    }
  }
}