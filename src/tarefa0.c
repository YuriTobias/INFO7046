/*
  Autor: Yuri Junqueira Tobias
  Data da ultima modificacao: 2026-05-31
  Descricao: primeiro programa de simulacao da disciplina de Sistemas Distribuidos.
  Vamos simular N processos, cada um conta o proprio tempo independentemente.
  Um exemplo simples e significativo para captar o espirito da simulacao.
*/

#include <stdio.h>
#include <stdlib.h>
#include "../smpl.h"

// EVENTOS
#define test 1
#define fault 2
#define recovery 3

// Estrutura para representar um processo do sistema distribuido
typedef struct {
    int id; // identificador de facility do SMPL
} processType;

processType *process;

int main(int argc, char *argv[]) {
  static int n; // numero de processos do Sistema Distribuido
  static int token; // identificador do processo que detem o token
  static int event, r, i;
  static int maxTimeSimulation = 150;

  static char fa_name[5];

  if (argc != 2) {
    printf("Usage: %s <number of processes>\n", argv[0]);
    exit(1);
  }

  n = atoi(argv[1]);

  smpl(0, "Token Ring Simulation");
  reset();
  stream(1);

  process = (processType *)malloc(n * sizeof(processType));

  for (i = 0; i < n; i++) {
    memset(fa_name, '\0', 5);
    sprintf(fa_name, "%d", i);
    process[i].id = facility(fa_name, 1);
  }

  // Escalonamento dos eventos iniciais
  // No primeiro intervalo de testes os processos vao testar
  for (i = 0; i < n; i++)
    schedule(test, 30.0, i); // Todos os processos de 0 ate n-1 vao testar na unidade de tempo 30.0
  
  schedule(fault, 31.0, 1);
  schedule(recovery, 61.0, 1);

  puts("===============================================================");
  puts("           Sistemas Distribuídos Prof. Elias");
  puts("          LOG do Trabalho Prático 0, Tarefa 0");
  puts("      Digitar, compilar e executar o programa tempo.c");
  printf("   Este programa foi executado para: N=%d processos.\n", n); 
  printf("           Tempo Total de Simulação = %d\n", maxTimeSimulation);
  puts("===============================================================");
  
  // Loop principal do simulador
  while (time() < maxTimeSimulation) {
    cause(&event, &token);
    switch (event) {
      case test:
        if (status(process[token].id) != 0) break;
        printf("Processo %d: estou testando no tempo %4.1f\n", token, time());
        schedule(test, 30.0, token);
        break;
      case fault:
        r = request(process[token].id, token, 0);
        printf("Socooorro!!! Sou o processo %d  e estou falhando no tempo %4.1f\n", token, time());
        break;
      case recovery:
        release(process[token].id, token);
        printf("Viva!!! Sou o processo %d e acabo de recuperar no tempo %4.1f\n", token, time());
        schedule(test, 1.0, token);
        break;
      default:
        printf("Unknown event type: %d\n", event);
    }
  }
}