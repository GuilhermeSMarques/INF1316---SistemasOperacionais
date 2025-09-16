#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

long long int somarTamanhoArquivos(const char *caminho) {
  DIR *dir;
  struct dirent *entrada;
  struct stat info;
  long long int totalTamanho = 0;

  dir = opendir(caminho);
  if (dir == NULL) {
    perror("Erro ao abrir diretório");
    return 0;
  }

  while ((entrada = readdir(dir)) != NULL) {
    if (strcmp(entrada->d_name, ".") == 0 ||
        strcmp(entrada->d_name, "..") == 0) {
      continue;
    }

    char novoCaminho[1024];
    snprintf(novoCaminho, sizeof(novoCaminho), "%s/%s", caminho,
             entrada->d_name);

    if (stat(novoCaminho, &info) == -1) {
      perror("Erro ao obter informações do arquivo");
      continue;
    }

    if (S_ISREG(info.st_mode)) {
      totalTamanho += info.st_size;
    } else if (S_ISDIR(info.st_mode)) {
      totalTamanho += somarTamanhoArquivos(novoCaminho);
    }
  }

  closedir(dir);

  return totalTamanho;
}

int main() {
  char caminhoCorrente[1024];

  if (getcwd(caminhoCorrente, sizeof(caminhoCorrente)) == NULL) {
    perror("Erro ao obter o diretório corrente");
    return 1;
  }

  long long int tamanhoTotal = somarTamanhoArquivos(caminhoCorrente);

  printf("O tamanho total dos arquivos no diretório '%s' e seus subdiretórios "
         "é: %lld bytes\n",
         caminhoCorrente, tamanhoTotal);

  return 0;
}