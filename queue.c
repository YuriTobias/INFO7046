/*
  Autor: Yuri Junqueira Tobias
  Disciplina: Sistemas Distribuidos. Prof. Elias
  Data da ultima modificacao: 2026-05-31
  Descricao: Fila para armazenar mensagens recebidas pelos processos.
*/

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void initQueue(messageQueue *q) {
    q->head = NULL;
    q->tail = NULL;
}

int isQueueEmpty(messageQueue *q) {
    return q->head == NULL;
}

void msgEnqueue(messageQueue *q, msgType element) {
    queueNode *newNode = (queueNode *)malloc(sizeof(queueNode));
    if (newNode == NULL) {
        fprintf(stderr, "Erro de alocação de memória na Fila!\n");
        exit(1);
    }
    newNode->data = element;
    newNode->next = NULL;
    
    if (q->tail == NULL) {
        q->head = newNode;
        q->tail = newNode;
    } else {
        q->tail->next = newNode;
        q->tail = newNode;
    }
}

msgType msgDequeue(messageQueue *q) {
    if (isQueueEmpty(q)) {
        msgType dummy = {-1, -1, -1};
        return dummy;
    }
    
    queueNode *temp = q->head;
    msgType data = temp->data;
    
    q->head = q->head->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    
    free(temp);
    return data;
}