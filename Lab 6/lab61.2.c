#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

int main() {
  int Fifo;
  char letra;

  if (access ("Fifo", F_OK) == -1) {
    if (mkfifo ("Fifo", S_IRUSR | S_IWUSR) != 0) {
      puts ("erro ao criar fifo");
      return -1;
    }
  }
  if ((Fifo = open ("Fifo", O_WRONLY)) < 0) {
    puts ("erro ao abrir fifo");
    return -1;
  }
  puts ("escrevendo letra por letra digitada");
  while (letra != '0') {
    scanf ("%c", &letra);
    write (Fifo, &letra, 1);
  }

  close (Fifo);

  return 0;
}