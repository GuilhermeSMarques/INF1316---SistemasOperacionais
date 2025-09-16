#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define FILE_PATH "shared_file.txt"
#define MESSAGE "Matheus Vilella esteve aqui!!!!"

int main() {
  int fd = open(FILE_PATH, O_RDWR);
  if (fd == -1) {
    perror("open");
    exit(1);
  }

  char *map = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (map == MAP_FAILED) {
    perror("mmap");
    close(fd);
    exit(1);
  }

  strncpy(map, MESSAGE, strlen(MESSAGE));
  printf("prog1: Escreveu a mensagem '%s' no arquivo mapeado.\n", MESSAGE);

  if (munmap(map, 1024) == -1) {
    perror("munmap");
  }
  close(fd);

  return 0;
}