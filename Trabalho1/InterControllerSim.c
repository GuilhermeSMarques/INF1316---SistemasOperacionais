#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void send_irq0(int pid) {
  printf("Enviando IRQ0 (TimeSlice)\n");
  kill(pid, SIGUSR1); // Envia o sinal de interrupção de time slice (SIGUSR1)
}

void send_irq1(int pid) {
  kill(pid, SIGUSR2); // Envia sinal de dispositivo D1
}

void send_irq2(int pid) {
  kill(pid, SIGHUP); // Envia sinal de dispositivo D2
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Uso: %s <PID_do_KernelSim>\n", argv[0]);
    return 1;
  }

  int kernel_pid = atoi(argv[1]);
  srand(time(NULL));

  while (1) {
    sleep(1); // Gera interrupções a cada 1 segundo para facilitar a visualização

    send_irq0(kernel_pid); // IRQ0 (TimeSlice)

    if ((rand() % 100) < 10) { // IRQ1 com probabilidade de 0.1
      send_irq1(kernel_pid);
    }

    if ((rand() % 100) < 5) { // IRQ2 com probabilidade de 0.05
      send_irq2(kernel_pid);
    }
  }

  return 0;
}