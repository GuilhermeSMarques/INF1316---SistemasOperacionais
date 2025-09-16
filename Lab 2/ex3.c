#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>


void buscarChave(int *vetor, int ini, int fim, int chave, int id) {
    for (int i = ini; i < fim; i++) {
        if (vetor[i] == chave) {
            printf("Processo %d encontrou a chave %d na posição %d.\n", id, chave, i);
            exit(0);
        }
    }
    printf("Processo %d não encontrou a chave %d.\n", id, chave);
    exit(0);
}

int main() 
{
    int shm_size = 20 * sizeof(int);

    int shmid = shmget(8752, shm_size, 0666 | IPC_CREAT);

    int *vetor = (int *)shmat(shmid, NULL, 0);

    for (int i = 0; i < 20; i++) 
    {
        vetor[i] = rand() % 50; // Números aleatórios entre 0 e 49
        printf("%d ", vetor[i]);
    }
    printf("\n");

    int chave;
    printf("Digite a chave a ser buscada: ");
    scanf("%d", &chave);

    int parte = 20 / 4;

    // Criação dos processos
    for (int i = 0; i < 4; i++) {
        pid_t pid = fork();
        
        if (pid == 0) 
        { // Processo filho
            int inicio = i * parte;
            int fim = inicio + parte; // Define o fim padrão para cada processo

            if (i == 4 - 1) // Se for o último processo, ajuste para cobrir até o fim do vetor
                fim = 20;
            buscarChave(vetor, inicio, fim, chave, i + 1);

        } 
        
        else if (pid < 0) 
        { // Erro ao criar processo
            perror("Erro ao criar processo");
            exit(1);
        }
    }

    for (int i = 0; i < 4; i++) 
    {
        wait(NULL);
    }

    shmdt(vetor);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
