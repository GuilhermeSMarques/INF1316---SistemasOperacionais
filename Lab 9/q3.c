#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct stat sb;
    off_t len;
    char *p;
    int fd;
    long page_size;
    long num_pages;
    struct timeval start, end;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        return 1;
    }

    if (!S_ISREG(sb.st_mode)) {
        fprintf(stderr, "%s is not a file\n", argv[1]);
        return 1;
    }

    page_size = sysconf(_SC_PAGESIZE);
    num_pages = (sb.st_size + page_size - 1) / page_size;
    printf("Tamanho do arquivo: %ld bytes\n", sb.st_size);
    printf("Tamanho da página: %ld bytes\n", page_size);
    printf("Número de páginas necessárias: %ld\n", num_pages);
    gettimeofday(&start, NULL);
    p = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    for (len = 0; len < sb.st_size; len++) {
        putchar(p[len]);
    }
    gettimeofday(&end, NULL);
    long elapsed_mapped =
        (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("\nTempo de leitura do arquivo mapeado: %ld microsegundos\n",
            elapsed_mapped);

    if (munmap(p, sb.st_size) == -1) {
        perror("munmap");
        return 1;
    }
    gettimeofday(&start, NULL);
    lseek(fd, 0, SEEK_SET);
    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);
    }
    gettimeofday(&end, NULL);
    long elapsed_disk =
        (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    printf("\nTempo de leitura do arquivo (E/S em disco): %ld microsegundos\n",
            elapsed_disk);

    if (close(fd) == -1) {
        perror("close");
        return 1;
    }

    return 0;
}