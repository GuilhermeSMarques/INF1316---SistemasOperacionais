#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TAMANHO_EXTRA 100

int main() {
    const char *arq = "arquivo.txt";

    int fd = open(arq, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    void *heap_inicio = sbrk(0);
    void *nova_alocacao = sbrk(TAMANHO_EXTRA);
    if (nova_alocacao == (void *)-1) {
        perror("Erro ao alocar memoria com sbrk");
        close(fd);
        return 1;
    }

    char *map = mmap(0, TAMANHO_EXTRA, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("Erro ao mapear o arquivo");
        close(fd);
        return 1;
    }

    strcpy(map, "Novos Dados no final do arquivo");

    // Desfaz o mapeamento e redefine o heap para o original
    if (munmap(map, TAMANHO_EXTRA) == -1) {
        perror("Erro ao desfazer o mapeamento");
    }
    if (brk(heap_inicio) != 0) {
        perror("Erro ao redefinir o limite do heap");
        close(fd);
        return 1;
    }

    close(fd);
    printf("Escrita no final do arquivo concluida\n");
    return 0;
}
