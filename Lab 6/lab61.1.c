#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
  int Fifo;
  char letra;
  if (access ("Fifo", F_OK) == -1) {
    if (mkfifo ("Fifo", S_IRUSR | S_IWUSR) != 0) {
        puts ("erro ao criar fifo");
      return -1;
    }
  }
  if ((Fifo = open ("Fifo", O_RDONLY)) < 0) {
    puts ("erro ao abrir fifo");
    return -1;
  }
  puts ("lendo");
  while (read (Fifo, &letra, sizeof (letra)) > 0) {
    printf("leu a letra: ");
    putchar (letra);
    printf("\n");
  }


  close(Fifo);
  return 0;
}