#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <sys/param.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAXPATHLEN 1024

int file_select(const struct direct *entry) {
    if ((entry->d_name[0] == '.')) return 0;
    return 1;
}

int main() {
    int count, i;
    struct direct **files;
    struct stat file_stat;
    char full_path[MAXPATHLEN];
    char cwd[MAXPATHLEN];
    time_t now;

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("Erro ao obter o caminho do diretório");
        exit(EXIT_FAILURE);
    }

    printf("Diretório Atual: %s\n", cwd);

    count = scandir(cwd, &files, file_select, alphasort);
    if (count < 0) {
        perror("Erro ao ler os arquivos do diretório");
        exit(EXIT_FAILURE);
    }

    if (count == 0) {
        printf("Nenhum arquivo encontrado.\n");
        exit(EXIT_SUCCESS);
    }

    printf("Número de arquivos: %d\n", count);

    time(&now);

    for (i = 0; i < count; i++) {
        snprintf(full_path, sizeof(full_path), "%s/%s", cwd, files[i]->d_name);

        if (stat(full_path, &file_stat) < 0) {
            perror("Erro ao obter informações do arquivo");
            continue;
        }

        double age_in_days = difftime(now, file_stat.st_mtime) / (60 * 60 * 24);

        printf("Arquivo: %s\n", files[i]->d_name);
        printf("  Inode: %lu\n", file_stat.st_ino);
        printf("  Tamanho: %lld bytes\n", (long long)file_stat.st_size);
        printf("  Idade: %.0f dias\n", age_in_days);
        printf("  Links: %lu\n", file_stat.st_nlink);
    }

    for (i = 0; i < count; i++) {
        free(files[i]);
    }
    free(files);

    return 0;
}
