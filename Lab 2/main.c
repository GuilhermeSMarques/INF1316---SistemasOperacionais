#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

void soma(int ini, int fim, int *p1, int *p2, int *p3){
    int i;
    for(i=ini;i<fim;i++){
        p3[i] = p1[i] + p2[i];
    }
}

int main (int argc, char *argv[]) 
{
    int segmento1, segmento2, segmento3, *p1, *p2, *p3, id, pid, status, i;
    
    segmento1 = shmget (IPC_PRIVATE, 9 * sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    segmento2 = shmget (IPC_PRIVATE, 9 * sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    segmento3 = shmget (IPC_PRIVATE, 9 * sizeof (int), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    
    p1 = (int *) shmat (segmento1, 0, 0);
    p2 = (int *) shmat (segmento2, 0, 0);
    p3 = (int *) shmat (segmento3, 0, 0);

    for(i=0;i<9;i++)
    {
        p1[i] = i;
        p2[i] = i;
        p3[i] = 0;
    }
    
    for(i=0;i<3;i++)
    {
        if ((id = fork()) < 0) 
        {
            printf("Erro na criação do novo processo");
            exit (-2);
        }
        else if (id == 0) //filho
        {
            soma(i*3,(i+1)*3, p1, p2, p3);
            sleep(1);
            return 0; // final
        }
        else //pai
            pid = wait (&status);
    }
    
    puts("Matriz:");

    for(i=0;i<3;i++) //print da terceira matriz após somatório
    {
        printf("%d %d %d \n", p3[3*i], p3[3*i+1], p3[3*i+2] );
    }
    
    // libera a memória compartilhada do processo
    shmdt (p1);
    shmdt (p2);
    shmdt (p3);
    
    // libera a memória compartilhada
    shmctl (segmento1, IPC_RMID, 0);
    shmctl (segmento2, IPC_RMID, 0);
    shmctl (segmento3, IPC_RMID, 0);
     
    return 0; 
}