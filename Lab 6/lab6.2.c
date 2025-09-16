#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

int main() {
    int fifo;
    char msg[30];
    int pid1, pid2;
    
    if (access("outrafifo", F_OK) == -1) {
        if (mkfifo("outrafifo", S_IRUSR | S_IWUSR) != 0) {
            puts("erro ao criar fifo");
            return -1;
        }
    }
    if ((pid1 = fork()) < 0) {
        puts("erro ao criar primeiro filho");
    } else if (pid1 == 0) {
        if ((fifo = open("outrafifo", O_WRONLY)) < 0) {
            puts("erro ao abrir fifo");
            return -1;
        }
        strcpy(msg, "começo da mensagem\n");
        write(fifo, msg, strlen(msg));
        close(fifo);
        return 0;
    } else {
        if ((pid2 = fork()) < 0) {
            puts("erro ao criar segundo filho");
        } else if (pid2 == 0) {
            if ((fifo = open("outrafifo", O_WRONLY)) < 0) {
                puts("erro ao abrir fifo");
                return -1;
            }
            strcpy(msg, "outra parte da mensagem\n");
            write(fifo, msg, strlen(msg));
            close(fifo);
            return 0;
        }
        else {
            if ((fifo = open("outrafifo", O_RDWR)) < 0) {
                puts("Erro ao abrir a FIFO para leitura e escrita");
                return -1;
            }
            int status;
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);
            while (read(fifo, msg, sizeof(msg)) > 0) {
                printf("%s", msg);
            }
            close(fifo);
        }
    }

    unlink("outrafifo");
    return 0;
}
