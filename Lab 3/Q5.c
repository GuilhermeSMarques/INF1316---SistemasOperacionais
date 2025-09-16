#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// Função para lidar com o sinal SIGFPE
void handle_sigfpe(int sig) {
    printf("Erro de ponto flutuante\n");
    exit(EXIT_FAILURE);
}

int main() {
    int num1, num2;
    int soma, sub, mult;
    float divisao;


    signal(SIGFPE, handle_sigfpe);

    printf("Digite o primeiro número: ");
    scanf("%d", &num1);
    printf("Digite o segundo número: ");
    scanf("%d", &num2);

    soma = num1 + num2;
    sub = num1 - num2;
    mult = num1 * num2;

    printf("Soma: %d + %d = %d\n", num1, num2, soma);
    printf("Subtração: %d - %d = %d\n", num1, num2, sub);
    printf("Multiplicação: %d * %d = %d\n", num1, num2, mult);

    // Verificar a divisão por zero
    if (num2 == 0) {
        printf("Divisão por zero detectada! Não é possível dividir por zero.\n");
    } else {
        divisao = (float)num1 / num2;
        printf("Divisão: %d / %d = %.2f\n", num1, num2, divisao);
    }

    return 0;
}
