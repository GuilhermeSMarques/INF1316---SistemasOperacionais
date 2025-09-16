#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


void ordena(int vetor[], int tam) {
    int aux;
    for (int x = 0; x < tam - 1; x++) {
        for (int y = x + 1; y < tam; y++) {
            if (vetor[x] > vetor[y]) {
                aux = vetor[x];
                vetor[x] = vetor[y];
                vetor[y] = aux;
            }
        }
    }
}


void imprime(int vetor[], int tam) {
    for (int i = 0; i < tam; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");
}


int main() {
    int vetor[10] = {6, 9, 3, 7, 2, 1, 8, 5, 10, 4}; 
    printf("Pai: valor do vetor antes do fork() = ");
    imprime(vetor, 10);

    pid_t pid = fork();

    if (pid < 0) {
        printf("Erro no processo");
        return 1;
    } 
    
    else if (pid == 0) {
        ordena(vetor, 10); 
        printf("Filho: valor do vetor depois de ordenar = ");
        imprime(vetor, 10);
    } 

    else {
        waitpid(pid, NULL, 0); 
        printf("Pai: valor do vetor depois da espera = ");
        imprime(vetor, 10);
    }

    return 0;
}