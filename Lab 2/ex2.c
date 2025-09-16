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
    
    segmento = shmget(8752, 30*sizeof (char), IPC_CREAT | S_IRUSR |  S_IWUSR);
    p = (char*) shmat(segmento, 0, 0);
    
    puts("Mensagem do dia:");
    fgets(p, 30, stdin);
    
    
    return 0;
}