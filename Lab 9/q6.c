#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TAMANHO_INICIAL 50
#define TAMANHO_EXPANDIDO 100

int main() {
    const char *nome_arquivo = "arquivo.txt";

    int fd = open(nome_arquivo, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    char *map = mmap(0, TAMANHO_INICIAL, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("Erro ao mapear o arquivo");
        close(fd);
        return 1;
    }

    strcpy(map, "Dados Iniciais");

    map = mremap(map, TAMANHO_INICIAL, TAMANHO_EXPANDIDO, MREMAP_MAYMOVE);
    if (map == MAP_FAILED) {
        perror("Erro ao redimensionar o mapeamento com mremap");
        close(fd);
        return 1;
    }

    strcat(map, " - Dados Adicionais");

    if (munmap(map, TAMANHO_EXPANDIDO) == -1) {
        perror("Erro ao desfazer o mapeamento");
    }

    close(fd);
    printf("Escrita expandida com sucesso.\n");
    return 0;
}
