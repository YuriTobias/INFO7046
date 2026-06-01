/*
  Autor: Yuri Junqueira Tobias
  Disciplina: Sistemas Distribuidos. Prof. Elias
  Data da ultima modificacao: 2026-05-31
  Descricao: ALGORITMO 2: Implemente o algoritmo aleatorizado para eleição de líder 
  (Randomized Leader Election Algorithm) baseado no anel (VRing).
*/

#include <stdio.h>
#include <stdlib.h>
#include "../smpl.h"
#include "../queue.h"

// EVENTOS
#define test 1
#define fault 2
#define recovery 3
#define drawBit 5
#define receiveElectionMsg 4

typedef enum {
  LEADER,
  UNKNOWN,
  FOLLOWER
} statusType;

// typedef struct {
//   int bid; // identificador do processo que está enviando a mensagem
//   int refId; // identificador do processo que é o remetente original da mensagem (para evitar loops infinitos)
//   int value; // id do processo que é o líder conhecido pelo processo que está enviando a mensagem
// } msgType;

// Estrutura para representar um processo do sistema distribuido
typedef struct {
  int id; // identificador de facility do SMPL
  int leader; // flag para indicar o id do processo líder
  int *candidates; // array de candidatos a líder
  statusType status; // flag para indicar se o processo é líder, desconhecido ou seguidor
  messageQueue msg_queue; // fila para armazenar mensagens recebidas
} processType;

processType *process;

int main(int argc, char *argv[]) {
  unsigned int seed;
  FILE *urandom = fopen("/dev/urandom", "r");
  if (urandom != NULL) {
      fread(&seed, sizeof(seed), 1, urandom);
      fclose(urandom);
  } else {
      seed = 12345; // fallback caso dê algum erro ao abrir o arquivo
  }
  srand(seed); // Define a semente aleatória real!

  static int n; // numero de processos do Sistema Distribuido
  static int token; // identificador do processo que detem o token
  static int event, r, i;

  static char fa_name[5];

  static int num_msgs = 0; // contador de mensagens enviadas durante a simulação
  static int num_rounds = 1; // contador de rodadas de sorteio de bits
  static int leader_elected = 0; // flag para indicar se um líder já foi eleito

  if (argc != 2) {
    printf("Usage: %s <number of processes>\n", argv[0]);
    exit(1);
  }

  n = atoi(argv[1]);

  if (n < 2) {
    printf("O número de processos deve ser pelo menos 2.\n");
    exit(1);
  }

  smpl(0, "Token Ring Simulation");
  reset();
  stream(1);

  process = (processType *)malloc(n * sizeof(processType));

  puts("====================================================================================================");
  puts("                                Sistemas Distribuidos Prof. Elias");
  puts("                             LOG do Trabalho Pratico 1, Algoritmo 2");
  puts("Algoritmo aleatorizado para eleição de líder (Randomized Leader Election Algorithm) baseado no anel.");
  printf("                       Este programa foi executado para: N=%d processos.\n", n);
  puts("====================================================================================================");

  for (i = 0; i < n; i++) {
    memset(fa_name, '\0', 5);
    sprintf(fa_name, "%d", i);
    process[i].id = facility(fa_name, 1);

    process[i].leader = -1; // O processo inicialmente não conhece nenhum líder
    process[i].status = UNKNOWN;
    initQueue(&process[i].msg_queue); // Inicializa a fila de mensagens para cada processo
  }

  printf("RODADA: %d\n", num_rounds);
  puts("====================================================================================================");

  // Escalonamento dos eventos iniciais:
  // Os processos sorteiam um bit aleatório (0 ou 1) para si mesmos e enviam esse bit para o próximo processo no anel
  for (i = 0; i < n; i++) {
    process[i].candidates = (int *)malloc(sizeof(int)*n);
    for (int j = 0; j < n; j++) {
      process[i].candidates[j] = -1;
    }
    process[i].candidates[i] = rand() % 2; // Cada processo sorteia um bit aleatório (0 ou 1) para si mesmo no início da simulação
    printf("[T=%3.1f] Proc %2d -> Sorteou bit inicial: %d\n", time(), i, process[i].candidates[i]);

    msgType msg;
    msg.bid = i;
    msg.refId = i;
    msg.value = process[i].candidates[i];

    msgEnqueue(&process[(i + 1) % n].msg_queue, msg);

    num_msgs++;

    printf("[T=%3.1f] Proc %2d -> Enviou mensagem inicial para Proc %2d (RefId: %2d, Bit: %d)\n", time(), i, (i + 1) % n, i, process[i].candidates[i]);
    schedule(receiveElectionMsg, 1.0, (i + 1) % n); // O processo agenda o recebimento da mensagem para o próximo processo no anel
  }
  
  // Loop principal do simulador
  while (!leader_elected) {
    cause(&event, &token);
    switch (event) {
      case drawBit:
        for (int j = 0; j < n; j++) {
          if (j != token && process[token].candidates[j] == 1)
            process[token].candidates[j] = -1; // O processo descarta os candidatos a líder que não são ele mesmo
        }

        if (process[token].candidates[token] == 1) {
          process[token].candidates[token] = rand() % 2; // O processo sorteia um bit aleatório (0 ou 1)
          printf("[T=%3.1f] Proc %2d -> Sorteou novo bit: %d\n", time(), token, process[token].candidates[token]);
        }

        msgType msg;
        msg.bid = token;
        msg.refId = token;
        msg.value = process[token].candidates[token];

        msgEnqueue(&process[(token + 1) % n].msg_queue, msg);

        num_msgs++;

        printf("[T=%3.1f] Proc %2d -> Encaminhou mensagem para Proc %2d (RefId: %2d, Bit: %d)\n", time(), token, (token + 1) % n, token, process[token].candidates[token]);
        schedule(receiveElectionMsg, 1.0, (token + 1) % n);
        if (token == n - 1) {
          num_rounds++; // Conta uma nova rodada de sorteio de bits a cada vez que uma mensagem completa um ciclo no anel
          puts("====================================================================================================");
          printf("RODADA: %d\n", num_rounds);
          puts("====================================================================================================");
        }
        break;
      case receiveElectionMsg: {
        msgType received_msg = msgDequeue(&process[token].msg_queue);
        
        printf("[T=%3.1f] Proc %2d -> Recebeu mensagem do Proc %2d (Origem/RefId: %2d, Valor: %d)\n", time(), token, received_msg.bid, received_msg.refId, received_msg.value);
        
        if (received_msg.refId != token) {
          process[token].candidates[received_msg.refId] = received_msg.value;
          
          if (process[token].candidates[received_msg.refId] == 1) {
            process[token].leader = received_msg.refId; // O processo atualiza o líder conhecido para o remetente original da mensagem
            process[token].status = FOLLOWER; // O processo se torna seguidor daquele líder conhecido
          }

          num_msgs++;

          msgType msg;
          msg.bid = token;
          msg.refId = received_msg.refId;
          msg.value = received_msg.value;

          msgEnqueue(&process[(token + 1) % n].msg_queue, msg);

          printf("[T=%3.1f] Proc %2d -> Repassou a mensagem para Proc %2d (RefId: %2d, Bit: %d)\n", time(), token, (token + 1) % n, received_msg.refId, received_msg.value);
          schedule(receiveElectionMsg, 1.0, (token + 1) % n);
          break;
        } else {       
          int num_candidates = 0;
          int num_unknown = 0;
          for (int j = 0; j < n; j++) {
            if (process[token].candidates[j] == 1) {
              num_candidates++;
            } else if (process[token].candidates[j] == -1) {
              num_unknown++;
            }
          }

          if (num_candidates == 1 && num_unknown == 0 && process[token].candidates[token] == 1) {
            printf("[T=%3.1f] Proc %2d -> IDENTIFICOU LÍDER ÚNICO! Fim da eleição.\n", time(), token);
            process[token].leader = token; // O processo se considera líder conhecido de si mesmo
            process[token].status = LEADER; // O processo se torna líder

            puts("====================================================================================================");
            printf("Número total de rodadas: %d\n", num_rounds);
            printf("Número total de mensagens enviadas: %d\n", num_msgs);
            printf("Tempo total de simulação até a eleição do líder: %4.1f\n", time());
            puts("====================================================================================================");

            leader_elected = 1;
            schedule(test, 30.0, token); // O líder inicia testes após ser eleito
          } else if (num_candidates == 0 && num_unknown == 0) {
            printf("[T=%3.1f] Proc %2d -> Nenhum candidato ativo restou nesta rodada. Reiniciando...\n", time(), token);
            process[token].candidates[token] = 1; // O processo recebe 1 para sortear um novo bit para si mesmo na próxima rodada
            schedule(drawBit, 0.0, token); // O processo agenda um novo sorteio de bit para si mesmo
          } else if (num_candidates > 1) {
            printf("[T=%3.1f] Proc %2d -> %d candidatos restantes. Reiniciando eleição...\n", time(), token, num_candidates);
            schedule(drawBit, 0.0, token); // O processo agenda um novo sorteio de bit para si mesmo
          }
        }}

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