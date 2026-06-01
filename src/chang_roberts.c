/*
  Autor: Yuri Junqueira Tobias
  Disciplina: Sistemas Distribuidos. Prof. Elias
  Data da ultima modificacao: 2026-05-31
  Descricao: ALGORITMO 1: Implemente o algoritmo de eleição de líder com candidatos baseado em Chang-Roberts no anel (VRing).
*/

#include <stdio.h>
#include <stdlib.h>
#include "../smpl.h"

// EVENTOS
#define test 1
#define fault 2
#define recovery 3
#define receiveElectionMsg 4

typedef enum {
  LEADER,
  UNKNOWN,
  FOLLOWER
} statusType;

typedef struct {
  int bid; // identificador do processo que está enviando a mensagem
  int value; // id do processo que é o líder conhecido pelo processo que está enviando a mensagem
} msgType;

// Estrutura para representar um processo do sistema distribuido
typedef struct {
  int id; // identificador de facility do SMPL
  int leader; // flag para indicar o id do processo líder
  msgType msg; // buffer para armazenar mensagens recebidas
  statusType status; // flag para indicar se o processo é líder, desconhecido ou seguidor
} processType;

processType *process;

int main(int argc, char *argv[]) {
  static int n; // numero de processos do Sistema Distribuido
  static int token; // identificador do processo que detem o token
  static int event, r, i;

  static char fa_name[5];

  static int num_msgs = 0; // contador de mensagens enviadas durante a simulação
  static int leader_elected = 0; // flag para indicar se um líder já foi eleito

  static int *candidates; // array para armazenar os candidatos a líder definidos pelo usuário
  static int num_candidates; // número de candidatos a líder definidos pelo usuário

  // Verificação dos argumentos de linha de comando
  if (argc != 2) {
    printf("Usage: %s <number of processes>\n", argv[0]);
    exit(1);
  }

  n = atoi(argv[1]);

  if (n < 2) {
    printf("O número de processos deve ser pelo menos 2.\n");
    exit(1);
  }

  // Alocação dinâmica do array de candidatos a líder
  candidates = (int *)malloc(n * sizeof(int));
  scanf("%d", &num_candidates);
  if (num_candidates < 1 || num_candidates > n) {
    printf("O número de candidatos deve ser entre 1 e %d.\n", n);
    exit(1);
  }
  
  for (int i = 0; i < num_candidates; i++) {
    scanf("%d", &candidates[i]);
    if (candidates[i] < 0 || candidates[i] >= n) {
      printf("ID de processo candidato inválido: %d. Deve ser entre 0 e %d.\n", candidates[i], n - 1);
      exit(1);
    }
  }

  smpl(0, "Token Ring Simulation");
  reset();
  stream(1);

  process = (processType *)malloc(n * sizeof(processType));

  for (i = 0; i < n; i++) {
    memset(fa_name, '\0', 5);
    sprintf(fa_name, "%d", i);
    process[i].id = facility(fa_name, 1);
    
    process[i].leader = -1; // O processo inicialmente não conhece nenhum líder
    process[i].status = UNKNOWN; // O processo inicialmente tem status desconhecido
  }

  puts("======================================================================================");
  puts("                             Sistemas Distribuidos Prof. Elias");
  puts("                           LOG do Trabalho Pratico 1, Algoritmo 1");
  puts("Algoritmo de eleição de líder com candidatos baseado em Chang-Roberts no anel (VRing).");
  printf("                      Este programa foi executado para: N=%d processos.\n", n);
  printf("                                 Candidatos iniciais: [");
  for (int j = 0; j < num_candidates; j++) {
    if (j == num_candidates - 1) {
      printf("%d]\n", candidates[j]);
    } else {
      printf("%d ", candidates[j]);
    }
  }
  puts("======================================================================================");

  // Escalonamento dos eventos iniciais: os processos candidatos iniciam a eleição enviando mensagens para seus vizinhos
  for (int j = 0; j < num_candidates; j++) {
    process[candidates[j]].leader = candidates[j]; // O processo se considera líder conhecido de si mesmo
    process[(candidates[j] + 1) % n].msg.bid = candidates[j]; // O processo envia sua própria ID como remetente da mensagem
    process[(candidates[j] + 1) % n].msg.value = candidates[j]; // O processo envia sua própria ID como candidato

    num_msgs++;
    
    printf("[T=%3.1f] Proc %2d -> Enviou candidatura (ID: %d) para o Proc %d\n", time(), candidates[j], candidates[j], (candidates[j] + 1) % n);
    schedule(receiveElectionMsg, 1.0, (candidates[j] + 1) % n); // O processo agenda o recebimento da mensagem para o próximo processo no anel
  }
  
  // Loop principal do simulador
  while (!leader_elected) {
    cause(&event, &token);
    switch (event) {
      case receiveElectionMsg:
        printf("[T=%3.1f] Proc %2d -> Recebeu mensagem do Proc %d (Candidato: %d)\n", time(), token, process[token].msg.bid, process[token].msg.value);

        // Se o ID na mensagem de eleição recebida for o mesmo que o ID do processo, esse processo passa a agir como líder.
        if (process[token].msg.value == token) {
          printf("[T=%3.1f] Proc %2d -> Identificou o próprio ID! LÍDER ELEITO!\n", time(), token);
          process[token].status = LEADER; // O processo se torna líder

          puts("======================================================================================");
          printf("Número total de mensagens enviadas: %d\n", num_msgs);
          printf("Tempo total de simulação até a eleição do líder: %.1f\n", time());
          puts("======================================================================================");

          leader_elected = 1;
          schedule(test, 30.0, token); // O líder inicia testes após ser eleito
          break;
        }

        // Se o ID na mensagem de eleição recebida for maior, o processo encaminha incondicionalmente a mensagem de eleição em direção horário.
        if (process[token].msg.value > process[token].leader) {
          printf("[T=%3.1f] Proc %2d -> Atualizou líder conhecido para %d\n", time(), token, process[token].msg.value);
          process[token].leader = process[token].msg.value; // O processo atualiza o líder conhecido
          process[token].status = FOLLOWER; // O processo se torna seguidor daquele líder conhecido
          process[(token + 1) % n].msg.bid = token; // O processo envia sua própria ID como remetente da mensagem
          process[(token + 1) % n].msg.value = process[token].msg.value; // O processo encaminha a mensagem recebida

          num_msgs++;

          printf("[T=%3.1f] Proc %2d -> Repassou candidatura (ID: %d) para o Proc %d\n", time(), token, process[token].msg.value, (token + 1) % n);
          schedule(receiveElectionMsg, 1.0, (token + 1) % n);
        } else {
          printf("[T=%3.1f] Proc %2d -> Descartou mensagem recebida (Candidato %d <= Líder %d)\n", time(), token, process[token].msg.value, process[token].leader);
        }

        break;
      case test:
        if (status(process[token].id) != 0) break;
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