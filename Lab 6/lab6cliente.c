#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#define FIFO_ENTRADA "fifo_entrada"
#define FIFO_SAIDA "fifo_saida"

int main() {
  int fd_entrada, fd_saida;
  char buffer[100];

  fd_entrada = open(FIFO_ENTRADA, O_WRONLY);
  if (fd_entrada < 0) {
    perror("erro ao abrir fifo");
    return -1;
  }

  printf("envie uma mensagem em letra minuscula: ");
  fgets(buffer, 100, stdin);
  write(fd_entrada, buffer, strlen(buffer) + 1);
  close(fd_entrada);
  fd_saida = open(FIFO_SAIDA, O_RDONLY);
  
  if (fd_saida < 0) {
    perror("erro ao abrir fifo");
    return -1;
  }

  if (read(fd_saida, buffer, sizeof(buffer)) > 0) {
    printf("retorno do servidor: %s\n", buffer);
  }
  
  close(fd_saida);
  return 0;
}
