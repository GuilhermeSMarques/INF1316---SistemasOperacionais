#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void listarDiretoriosArquivos(const char *caminho, int nivel) {
  DIR *dir;
  struct dirent *entrada;
  struct stat info;

  dir = opendir(caminho);
  if (dir == NULL) {
    perror("Erro ao abrir diretório");
    return;
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

    printf("%*s[%s]\n", nivel * 2, "", entrada->d_name);

    if (S_ISDIR(info.st_mode)) {
      listarDiretoriosArquivos(novoCaminho, nivel + 1);
    }
  }
  closedir(dir);
}

int main() {
  char caminhoCorrente[1024];
  if (getcwd(caminhoCorrente, sizeof(caminhoCorrente)) == NULL) {
    perror("Erro ao obter o diretório corrente");
    return 1;
  }

  printf("Conteúdo do diretório '%s':\n", caminhoCorrente);
  listarDiretoriosArquivos(caminhoCorrente, 0);

  return 0;
}