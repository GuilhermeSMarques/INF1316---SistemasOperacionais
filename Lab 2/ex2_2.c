#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) 
{
    int segmento;
    char * p;
    
    segmento = shmget (8752, 30 * sizeof(char), IPC_EXCL | S_IRUSR |  S_IWUSR);
    p = (char*) shmat(segmento, 0, 0);
    
    printf("A mensagem do dia é: %s\n", p);
    
    shmdt(p);
    shmctl(segmento, IPC_RMID, 0);
    
    return 0;
}