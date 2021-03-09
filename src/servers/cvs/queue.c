#include "inc.h"

void enqueue(struct queue* q, endpoint_t val){
    q->T[q->tail] = val;
    q->tail++;
    q->tail %= NR_PROCS;
}

endpoint_t dequeue(struct queue* q){
    endpoint_t temp = q->T[q->head];
    q->head++;
    q->head %= NR_PROCS;
    return temp;
}

endpoint_t peek(struct queue* q){
    return q->T[q->head];
}

char isEmpty(struct queue* q){
    return q->head == q->tail;
}

void enCVQueue(struct CVQueue* q, endpoint_t val, int mut){
    q->T[q->tail] = val;
    q->S[q->tail] = mut;
    q->tail++;
    q->tail %= NR_PROCS;
}

void deCVQueue(struct CVQueue* q, endpoint_t* val, int* mut){
    *val = q->T[q->head];
    *mut = q->S[q->head];
    q->head++;
    q->head %= NR_PROCS;
}

char isCVQueueEmpty(struct CVQueue* q){
    return q->head == q->tail;
}