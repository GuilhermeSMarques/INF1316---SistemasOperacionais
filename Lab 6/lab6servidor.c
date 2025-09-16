#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#define FIFO_ENTRADA "fifo_entrada"
#define FIFO_SAIDA "fifo_saida"


void Maiusculas(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int main() {
    int fd_entrada, fd_saida;
    char buffer[100];
    
    if (access(FIFO_ENTRADA, F_OK) == -1) {
        mkfifo(FIFO_ENTRADA, 0666);
    }
    if (access(FIFO_SAIDA, F_OK) == -1) {
        mkfifo(FIFO_SAIDA, 0666);
    }
    printf("servidor esta rodando\n");

    while (1) {
        fd_entrada = open(FIFO_ENTRADA, O_RDONLY);
        if (fd_entrada < 0) {
            perror("erro ao abrir fifo");
            exit(1);
        }

        if (read(fd_entrada, buffer, sizeof(buffer)) > 0) {
            printf("servidor recebeu: %s\n", buffer);
            Maiusculas(buffer);
            fd_saida = open(FIFO_SAIDA, O_WRONLY);
            if (fd_saida < 0) {
                perror("erro ao abrir fifo");
                exit(1);
            }
            write(fd_saida, buffer, strlen(buffer) + 1);
            close(fd_saida);
        }
        close(fd_entrada);
    }
    return 0;
}