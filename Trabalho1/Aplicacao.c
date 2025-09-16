#include "syscall.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int PC = 1;
  srand(getpid());

  int mem_id = shmget(9100, sizeof(Syscall), IPC_CREAT | 0666);
  Syscall *buffer = shmat(mem_id, 0, 0);

  while (1) { // Loop executa até PC atingir o valor máximo
    sleep(1); // Simula execução
    printf("Processo %d: Iteração = %d\n", getpid(), PC);

    if ((rand() % 100) < 15) { // 15% de chance de syscall
      int dispositivo = rand() % 2 + 1;
      printf("Processo %d: syscall no dispositivo D%d\n", getpid(),dispositivo);
      
      int operacao_int = rand() % 3;
      char operations[3] = {'r', 'w', 'x'};
      char operacao = operations[operacao_int];
      Syscall syscall;
      syscall.pid = getpid();
      syscall.dispositivo = dispositivo;
      syscall.tipo_de_operacao = operacao;
      *buffer = syscall;
      kill(getppid(), SIGQUIT); // Sending syscall signal to Kernel
    } 

    else {
      Syscall pc_update;
      pc_update.pid = getpid();
      pc_update.dispositivo = -1;
      pc_update.tipo_de_operacao = -1;

      *buffer = pc_update;
      kill(getppid(), SIGQUIT);
    }
    PC++;
  }

  printf("Processo %d terminou\n", getpid()); // Mensagem de término
  return 0;
}