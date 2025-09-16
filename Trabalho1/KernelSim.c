#include "process.h"
#include "syscall.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_PROCESSOS 3 // Número de processos de aplicação
#define MAX_FILA 5      // Tamanho máximo das filas de bloqueio
#define MAX_ITERACOES 100 // Número máximo de iterações

Process processos[NUM_PROCESSOS];
int processo_atual = 0;
int processos_ativos = NUM_PROCESSOS;
int iteracoes[NUM_PROCESSOS];
int mem_id;
int intercontroller_pid;
Syscall *buffer;

// Estrutura da fila de bloqueio
typedef struct {
  pid_t fila[MAX_FILA];
  int inicio, fim, tamanho;
} FilaBloqueio;

FilaBloqueio fila_d1, fila_d2; // Fila de bloqueio para dispositivos D1 e D2

void configurar_sinais();
Process *get_process(int pid);

// Função para tratar interrupções e encerrar o programa
void tratar_interrupcao(int sig) {
  printf("\nSinal de interrupção recebido. Encerrando o programa...\n");
  shmdt(buffer);
  shmctl(mem_id, IPC_RMID, 0);
  exit(0);
}

void inicializar_fila(FilaBloqueio *fila) {
  fila->inicio = 0;
  fila->fim = 0;
  fila->tamanho = 0;
}

int adicionar_fila(FilaBloqueio *fila, pid_t processo) {
  if (fila->tamanho == MAX_FILA)
    return -1;
  fila->fila[fila->fim] = processo;
  fila->fim = (fila->fim + 1) % MAX_FILA;
  fila->tamanho++;
  return 0;
}

pid_t remover_fila(FilaBloqueio *fila) {
  if (fila->tamanho == 0)
    return -1;
  pid_t processo = fila->fila[fila->inicio];
  fila->inicio = (fila->inicio + 1) % MAX_FILA;
  fila->tamanho--;
  return processo;
}

void iniciar_processos() {
  for (int i = 0; i < NUM_PROCESSOS; i++) {

    pid_t pid = fork();
    if (pid == 0) {
      execlp("./aplicacao", "aplicacao", NULL);
      exit(0);
    } else {
      printf("Processo A%d iniciado com PID: %d\n", i + 1, pid);
      kill(pid, SIGSTOP);

      processos[i].pid = pid;
      processos[i].state = Ready;
      processos[i].d1_count = 0;
      processos[i].d2_count = 0;
      processos[i].pc = 0;
      processos[i].op_block = -1;
    }
  }
}

int processo_esta_bloqueado(pid_t processo) {
  for (int i = 0; i < fila_d1.tamanho; i++) {
    if (fila_d1.fila[(fila_d1.inicio + i) % MAX_FILA] == processo) {
      return 1; // Processo está bloqueado na fila do dispositivo D1
    }
  }
  for (int i = 0; i < fila_d2.tamanho; i++) {
    if (fila_d2.fila[(fila_d2.inicio + i) % MAX_FILA] == processo) {
      return 1; // Processo está bloqueado na fila do dispositivo D2
    }
  }
  return 0; // Processo não está bloqueado
}

void print_estado() {
  printf("\nEstado atual:\n");
  for (int i = 0; i < NUM_PROCESSOS; i++) {
    Process p = processos[i];
    printf("Processo %d:\n", i);
    printf(" - PID: %d\n", p.pid);
    switch (p.state) {
    case Running:
      printf(" - ESTADO: Rodando\n");
      break;
    case Ready:
      printf(" - ESTADO: Pronto\n");
      break;
    case Blocked_1:
      printf(" - ESTADO: Bloqueado no dispositivo D1\n");
      printf(" - OPERAÇÃO: %c\n", p.op_block);
      break;
    case Blocked_2:
      printf(" - ESTADO: Bloqueado no dispositivo D2\n");
      printf(" - OPERAÇÃO: %c\n", p.op_block);
      break;
    case Terminated:
      printf(" - ESTADO: Terminado\n");
      break;
    }
    printf(" - PC: %d\n", p.pc);
    printf(" - D1_COUNT: %d\n", p.d1_count);
    printf(" - D2_COUNT: %d\n\n", p.d2_count);
  }
}

void syscall_bloquear_processo(Process *p, int dispositivo, char operation) {
  if (dispositivo == 1) {
    p->d1_count++;
    p->state = Blocked_1;
    adicionar_fila(&fila_d1, p->pid);
    printf("Processo %d bloqueado no dispositivo D1\n", p->pid);
  } else if (dispositivo == 2) {
    p->d2_count++;
    p->state = Blocked_2;
    adicionar_fila(&fila_d2, p->pid);
    printf("Processo %d bloqueado no dispositivo D2\n", p->pid);
  }
  p->op_block = operation;
  kill(p->pid, SIGSTOP); // Pausa o processo
}

void increment_pc(Process *p) {}

void desbloquear_processo(int dispositivo) {
  pid_t processo;
  if (dispositivo == 1 && fila_d1.tamanho > 0) {
    processo = remover_fila(&fila_d1);
    if (processo != -1) {
      Process *p = get_process(processo);
      p->state = Running;
      p->op_block = -1;
      printf("Desbloqueando processo %d do dispositivo D1\n", processo);
      kill(processos[processo_atual].pid, SIGSTOP);
      kill(processo, SIGCONT); // Retoma o processo
    }
  } else if (dispositivo == 2 && fila_d2.tamanho > 0) {
    processo = remover_fila(&fila_d2);
    if (processo != -1) {
      Process *p = get_process(processo);
      p->state = Running;
      p->op_block = -1;
      printf("Desbloqueando processo %d do dispositivo D2\n", processo);
      kill(processos[processo_atual].pid, SIGSTOP);
      kill(processo, SIGCONT); // Retoma o processo
    }
  } else {
    printf("Nenhum processo na fila para o dispositivo %d\n", dispositivo);
  }
}

Process *get_running_process() {
  for (int i = 0; i < NUM_PROCESSOS; i++) {
    if (processos[i].state == Running) {
      return processos + i;
    }
  }
  return NULL;
}

void trocar_processo() {
  if (processos_ativos == 0) {
    printf("Todos os processos terminaram. KernelSim encerrando.\n");
    exit(0);
  }

  printf("Tentando trocar para o próximo processo...\n");
  int proximo_processo = (processo_atual + 1) % NUM_PROCESSOS;
  int tentativas = 0;
  while (tentativas <= NUM_PROCESSOS &&
         (processos[proximo_processo].pid == -1 ||
          processos[proximo_processo].state != Ready)) {
    proximo_processo = (proximo_processo + 1) % NUM_PROCESSOS;
  }
  if (tentativas <= NUM_PROCESSOS) {
    Process *current_p = get_running_process();
    if (current_p != NULL) {
      current_p->state = Ready;
    }
    processos[proximo_processo].state = Running;
    kill(processos[processo_atual].pid, SIGSTOP);
    kill(processos[proximo_processo].pid, SIGCONT);
    processo_atual = proximo_processo;
    printf("Troca concluída. Pausando processo atual e ativando o próximo.\n");
  } else {
    printf("Nenhum processo pronto");
  }
}

void verificar_terminado() {
  if (processos[processo_atual].pc >= MAX_ITERACOES) {
  }
}

void handle_irq0(int sig) {
  printf("Recebido IRQ0 (Time Slice)\n");
  trocar_processo();
}

void handle_irq1(int sig) {
  printf("Recebido IRQ1 (Dispositivo D1)\n");
  desbloquear_processo(1);
}

void handle_irq2(int sig) {
  printf("Recebido IRQ2 (Dispositivo D2)\n");
  desbloquear_processo(2);
}

Process *get_process(int pid) {
  for (int i = 0; i < NUM_PROCESSOS; i++) {
    if (processos[i].pid == pid) {
      return processos + i;
    }
  }
  return NULL;
}

void terminate(Process *p) {
  if (p->state == Running) {
    kill(p->pid, SIGSTOP);
  }
  p->state = Terminated;
  for (int i = 0; i < fila_d1.tamanho; i++) {
    if (fila_d1.fila[i] == p->pid) {
      fila_d1.fila[i] = -1;
    }
  }

  for (int i = 0; i < fila_d2.tamanho; i++) {
    if (fila_d2.fila[i] == p->pid) {
      fila_d2.fila[i] = -1;
    }
  }
  printf("O processo %d foi terminado\n", p->pid);
}

void handle_syscall() {

  Syscall sc = *buffer;

  // Print the received message

  Process *p = get_process(sc.pid);

  p->pc++;
  if (p->pc >= MAX_ITERACOES) {
    terminate(p);
    return;
  }
  if (sc.dispositivo != -1) {
    printf("Syscall recebida: Processo: %d; Dispositivo: %d; Operação: %c\n",
           sc.pid, sc.dispositivo, sc.tipo_de_operacao);
    syscall_bloquear_processo(p, sc.dispositivo, sc.tipo_de_operacao);
  }
}

void signal_deactivated() { printf("SIGNAL_DEACTIVATED\n"); }

void handle_pause() {
  signal(SIGUSR1, signal_deactivated);
  signal(SIGUSR2, signal_deactivated);
  signal(SIGHUP, signal_deactivated);
  signal(SIGQUIT, signal_deactivated);
  signal(SIGTERM, signal_deactivated);

  print_estado();
  kill(intercontroller_pid, SIGSTOP);           // pause intercontroller
  kill(processos[processo_atual].pid, SIGSTOP); // pause running process
}

void handle_resume() {
  configurar_sinais();

  kill(intercontroller_pid, SIGCONT);
  Process *current_p = get_running_process();
  if (current_p != NULL) {
    kill(current_p->pid, SIGCONT);
  }
}

void configurar_sinais() {
  signal(SIGUSR1, handle_irq0);
  signal(SIGUSR2, handle_irq1);
  signal(SIGHUP, handle_irq2);
  signal(SIGQUIT, handle_syscall);
  signal(SIGTERM, handle_pause);
  signal(SIGABRT, handle_resume);
}

void start_execution() {
  kill(processos[0].pid, SIGCONT); // Inicia o processo 0
  processo_atual = 0;
}

int main() {
  printf("Iniciando Kernel...\n");
  printf("KERNEL_PID: %d\n", getpid());
  mem_id = shmget(9100, sizeof(Syscall), IPC_CREAT | 0666);
  buffer = shmat(mem_id, 0, 0);
  signal(SIGINT, tratar_interrupcao);
  inicializar_fila(&fila_d1);
  inicializar_fila(&fila_d2);

  // Start Intercontroller
  intercontroller_pid = fork();
  if (intercontroller_pid == 0) {
    char kernel_pid[10];
    sprintf(kernel_pid, "%d", getppid());
    execlp("./InterControllerSim", "InterControllerSim", kernel_pid, NULL);
    exit(0);
  }
  printf("INTERCONTROLLER_PID: %d\n", intercontroller_pid);

  // Starting CLI
  int cli_pid = fork();
  if (cli_pid == 0) {
    char kernel_pid[10];
    sprintf(kernel_pid, "%d", getppid());
    execlp("./cli", "cli", kernel_pid, NULL);
    exit(0);
  }
  printf("CLI_PID: %d\n", cli_pid);
  configurar_sinais();
  iniciar_processos();
  printf("Processos iniciados. KernelSim iniciado.\n");
  start_execution();

  while (processos_ativos > 0) {
    pause(); // Espera até receber um sinal
  }

  printf("Todos os processos terminaram. KernelSim encerrado.\n");
  return 0;
}
