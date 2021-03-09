#include "inc.h"

void giveMutex(endpoint_t ep, message* m){
    m->m_type = 0;
    int r;
    if ((r = sendnb(ep, m)) != OK)
        printf("CVS send error %d.\n", r);
}

int binSearch(int* T, int head, int tail, int val, int size){
    if (head == tail)
        return -1;
    int p = head, q = (tail == 0 ?  size - 1 : tail - 1);
    if (p>q)
        q+=size;
    while (p<q){
        int s = (p+q)/2;
        if (T[(s%size)] >= val)
            q = s;
        else
            p = s + 1;
    }
    return p;
}

void swapMutexes(int x, int y){
    int temp = mutexes[x];
    struct queue tempQueue = mutexQueues[x];
    mutexes[x] = mutexes[y];
    mutexQueues[x] = mutexQueues[y];
    mutexes[y] = temp;
    mutexQueues[y] = tempQueue;
}

void swapCVS(int x, int y){
    int temp = conditionVariables[x];
    struct CVQueue tempQueue = CVQueues[x];
    conditionVariables[x] = conditionVariables[y];
    CVQueues[x] = CVQueues[y];
    conditionVariables[y] = temp;
    CVQueues[y] = tempQueue;
}

void swap(int x, int y, char isMutex){
    if (isMutex)
        swapMutexes(x, y);
    else
        swapCVS(x, y);
}

int shiftLeft(int head, int tail, int val, int size, char isMutex){
    int nextI;
    if (isMutex)
        mutexes[(head == 0 ?  size - 1 : head - 1)] = val;
    else
        conditionVariables[(head == 0 ?  size - 1 : head - 1)] = val;
    for (int i=(head == 0 ?  size - 1 : head - 1);;){
        nextI = (i+1)%size;
        if (nextI == tail || (isMutex && mutexes[nextI]>val) || (!isMutex && conditionVariables[nextI]>val))
            return i;
        swap(i, nextI, isMutex);
        i = nextI;
    }
}

int shiftRight(int head, int tail, int val, int size, char isMutex) {
    int nextI;
    if (isMutex)
        mutexes[tail] = val;
    else
        conditionVariables[tail] = val;
    for (int i=tail;;){
        nextI = (i == 0 ? size - 1 : i - 1);
        if (i == head || (isMutex && mutexes[nextI]<val) || (!isMutex && conditionVariables[nextI]<val))
            return i;
        swap(i, nextI, isMutex);
        i = nextI;
    }
}

int removeFromT(int head, int tail, int idx, int size, char isMutex){
    int op = head, oq = (tail == 0 ?  size - 1 : tail - 1);
    if (op>oq)
        oq+=size;
    if (idx-op<oq-idx){
        for (int i=idx; i!=head;){
            swap(i, (i == 0 ? size - 1 : i - 1), isMutex);
            i = (i == 0 ? size - 1 : i - 1);
        }
        return 1;
    }
    else{
        for (int i=idx; ((i+1)%size)!=tail;){
            swap(i, (i+1)%size, isMutex);
            i = (i+1)%size;
        }
        return 0;
    }
}

int findMutex(int mutex){
    int p = binSearch(mutexes, mutexesHead, mutexesTail, mutex, MUTEX_NR);
    if (p == -1)
        return -1;
    if (mutexes[(p%MUTEX_NR)] == mutex)
        return p%MUTEX_NR;
    return -1;
}

int createMutex(int mutex){
    int op = mutexesHead, oq = (mutexesTail == 0 ?  MUTEX_NR - 1 : mutexesTail - 1);
    if (op>oq)
        oq+=MUTEX_NR;
    int p = binSearch(mutexes, mutexesHead, mutexesTail, mutex, MUTEX_NR);
    if (p == -1){
        mutexes[mutexesHead] = mutex;
        mutexesTail++;
        mutexesTail%=MUTEX_NR;
        return mutexesHead;
    }
    if (p-op<oq-p){
        p = shiftLeft(mutexesHead, mutexesTail, mutex, MUTEX_NR, 1);
        mutexesHead = (mutexesHead == 0 ? MUTEX_NR - 1 : mutexesHead - 1);
    }
    else{
        p = shiftRight(mutexesHead, mutexesTail, mutex, MUTEX_NR, 1);
        mutexesTail++;
        mutexesTail %= MUTEX_NR;
    }
    return p;
}

void removeMutex(int idx){
    if (removeFromT(mutexesHead, mutexesTail, idx, MUTEX_NR, 1))
        mutexesHead = (mutexesHead + 1)%MUTEX_NR;
    else
        mutexesTail = (mutexesTail == 0 ? MUTEX_NR - 1 : mutexesTail - 1);
}

int findCV(int cv){
    int p = binSearch(conditionVariables, cvsHead, cvsTail, cv, NR_PROCS);
    if (p == -1)
        return -1;
    if (conditionVariables[(p%NR_PROCS)] == cv)
        return p%NR_PROCS;
    return -1;
}

int createCV(int cv){
    int op = cvsHead, oq = (cvsTail == 0 ? NR_PROCS - 1 : cvsTail - 1);
    if (op>oq)
        oq+=NR_PROCS;
    int p = binSearch(conditionVariables, cvsHead, cvsTail, cv, NR_PROCS);
    if (p == -1){
        conditionVariables[cvsHead] = cv;
        cvsTail++;
        cvsTail%=NR_PROCS;
        return cvsHead;
    }
    if (p-op<oq-p){
        p = shiftLeft(cvsHead, cvsTail, cv, NR_PROCS, 0);
        cvsHead = (cvsHead == 0 ? NR_PROCS - 1 : cvsHead - 1);
    }
    else{
        p = shiftRight(cvsHead, cvsTail, cv, NR_PROCS, 0);
        cvsTail++;
        cvsTail %= NR_PROCS;
    }
    return p;
}

void removeCV(int idx){
    if (removeFromT(cvsHead, cvsTail, idx, NR_PROCS, 0))
        cvsHead = (cvsHead + 1)%NR_PROCS;
    else
        cvsTail = (cvsTail == 0 ? NR_PROCS - 1 : cvsTail - 1);
}

int lookThroughQueue(int idx, endpoint_t ep){
    for (int i=mutexQueues[idx].head; i!=mutexQueues[idx].tail;){
        if (mutexQueues[idx].T[i] == ep)
            return i;
        i++;
        i%=NR_PROCS;
    }
    return -1;
}

int lookThroughCVQueue(int idx, endpoint_t ep){
    for (int i=CVQueues[idx].head; i!=CVQueues[idx].tail;){
        if (CVQueues[idx].T[i] == ep)
            return i;
        i++;
        i%=NR_PROCS;
    }
    return -1;
}
