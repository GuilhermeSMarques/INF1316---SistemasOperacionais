#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

// Variáveis globais para controlar o tempo
time_t tempo_inicio;
int chamada = 0; // 0 = nenhuma chamada, 1 = chamada em andamento

float calcular_custo(time_t duracao) {
    float custo = 0.0;
    if (duracao <= 60) {
        custo = duracao * 0.02; // 2 centavos por segundo até 1 minuto
    } else {
        custo = (60 * 0.02) + ((duracao - 60) * 0.01); // 2 centavos até 1 minuto + 1 centavo após
    }
    return custo;
}

//início da chamada (SIGUSR1)
void handle_sigusr1(int sig) {
    if (!chamada) {
        printf("Iniciando chamada...\n");
        tempo_inicio = time(NULL); // Marcar o tempo de início
        chamada = 1;
    } else {
        printf("Chamada já está em andamento.\n");
    }
}

//término da chamada (SIGUSR2)
void handle_sigusr2(int sig) {
    if (chamada) {
        time_t tempo_fim = time(NULL); 
        time_t duracao = tempo_fim - tempo_inicio; 
        float custo = calcular_custo(duracao); 
        printf("Chamada finalizada. Duração: %ld segundos. Custo: R$%.2f\n", duracao, custo);
        chamada = 0;
    } else {
        printf("Nenhuma chamada em andamento para finalizar.\n");
    }
}

int main() {

    signal(SIGUSR1, handle_sigusr1);
    signal(SIGUSR2, handle_sigusr2);

    printf("Programa em execução em background. PID: %d\n", getpid());
    

    while (1) {
        pause();
    }

    return 0;
}
