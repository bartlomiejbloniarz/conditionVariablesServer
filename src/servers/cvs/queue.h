struct queue{
    endpoint_t T[NR_PROCS];
    int head, tail;
};

void enqueue(struct queue* q, endpoint_t val);

endpoint_t dequeue(struct queue* q);

endpoint_t peek(struct queue* q);

char isEmpty(struct queue* q);

struct CVQueue{
    endpoint_t T[NR_PROCS];
    int S[NR_PROCS];
    int head, tail;
};

void enCVQueue(struct CVQueue* q, endpoint_t val, int mut);

void deCVQueue(struct CVQueue* q, endpoint_t* val, int* mut);

char isCVQueueEmpty(struct CVQueue* q);
