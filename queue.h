/*
  Autor: Yuri Junqueira Tobias
  Disciplina: Sistemas Distribuidos. Prof. Elias
  Data da ultima modificacao: 2026-05-31
  Descricao: Fila para armazenar mensagens recebidas pelos processos.
*/

#ifndef QUEUE_H
#define QUEUE_H

// A estrutura da mensagem migra para cá
typedef struct {
    int bid;   // identificador do processo que está enviando a mensagem
    int refId; // identificador do processo que é o remetente original
    int value; // id do processo líder conhecido
} msgType;

// Nó da fila encadeada
typedef struct queueNode {
    msgType data;
    struct queueNode *next;
} queueNode;

// Estrutura de controle da Fila
typedef struct {
    queueNode *head;
    queueNode *tail;
} messageQueue;

// Assinaturas das funções que serão implementadas no .c
void initQueue(messageQueue *q);
int isQueueEmpty(messageQueue *q);
void msgEnqueue(messageQueue *q, msgType element);
msgType msgDequeue(messageQueue *q);

#endif