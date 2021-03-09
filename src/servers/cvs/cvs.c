#include "inc.h"

/*===========================================================================*
 *                                  lock                                     *
 *===========================================================================*/

int lock(message* m){
    if (m->m1_i1 != -1) {
        int mutex = m->m1_i1, idx = findMutex(mutex);
        if (idx == -1) {
            idx = createMutex(mutex);
            mutexQueues[idx].head = 0;
            mutexQueues[idx].tail = 0;
            giveMutex(who_e, m);
        } else {
            if (peek(&mutexQueues[idx]) == who_e)
                return EPERM;
        }
        enqueue(&mutexQueues[idx], who_e);
    }
    return 0;
}

/*===========================================================================*
 *                                 unlock                                    *
 *===========================================================================*/

int doUnlock(int idx, message* m){
    dequeue(&mutexQueues[idx]);
    if (isEmpty(&mutexQueues[idx]))
        removeMutex(idx);
    else
        giveMutex(peek(&mutexQueues[idx]), m);
    return 0;
}

int unlock(message* m){
    int mutex = m->m1_i1, idx = findMutex(mutex);
    if (idx == -1)
        return EPERM;
    if (who_e == peek(&mutexQueues[idx])){
        return doUnlock(idx, m);
    }
    return EPERM;
}

/*===========================================================================*
 *                                  wait                                     *
 *===========================================================================*/

int wait(message* m){
    if (m->m1_i3 == 0) {
        int mutex = m->m1_i1, cv = m->m1_i2, idx = findMutex(mutex), CVIdx = findCV(cv);
        if (who_e == peek(&mutexQueues[idx])) {
            doUnlock(idx, m);
            if (CVIdx == -1) {
                CVIdx = createCV(cv);
                CVQueues[CVIdx].head = 0;
                CVQueues[CVIdx].tail = 0;
            }
            enCVQueue(&CVQueues[CVIdx], who_e, mutex);
            return 0;
        }
        return EINVAL;
    }
    else if (m->m1_i3 == 1){
        int cv = m->m1_i2, CVIdx = findCV(cv);
        int result = lookThroughCVQueue(CVIdx, who_e);
        if (result != -1) {
            for (int k=result; (k+1)%NR_PROCS != CVQueues[CVIdx].tail;) {
                CVQueues[CVIdx].T[k] = CVQueues[CVIdx].T[(k + 1) % NR_PROCS];
                k++;
                k %= NR_PROCS;
            }
            if (CVQueues[CVIdx].tail == 0)
                CVQueues[CVIdx].tail = NR_PROCS - 1;
            else
                CVQueues[CVIdx].tail--;
            if (CVQueues[CVIdx].head == CVQueues[CVIdx].tail)
                removeCV(CVIdx);
            lock(m);
        }
    }
    return 0;
}

/*===========================================================================*
 *                               broadcast                                   *
 *===========================================================================*/

int broadcast(message* m){
    int cv = m->m1_i2, CVIdx = findCV(cv), mutex;
    endpoint_t who, oldWho;
    if (CVIdx != -1){
        oldWho = who_e;
        while (!isCVQueueEmpty(&CVQueues[CVIdx])) {
            deCVQueue(&CVQueues[CVIdx], &who, &mutex);
            who_e = who;
            m->m1_i1 = mutex;
            lock(m);
        }
        who_e = oldWho;
        removeCV(CVIdx);
    }
    return 0;
}

/*===========================================================================*
 *                            processUnpause                                 *
 *===========================================================================*/

int processUnpause(message* m){
    endpoint_t who = m->PM_PROC;
    int result = -1;
    for (int i=mutexesHead; i!=mutexesTail; i=(i+1)%MUTEX_NR){
        result = lookThroughQueue(i, who);
        if (result != -1 && result != mutexQueues[i].head) {
            who_e = who;
            return EINTR;
        }
    }
    for (int i=cvsHead; i!=cvsTail; i=(i+1)%NR_PROCS){
        result = lookThroughCVQueue(i, who);
        if (result != -1) {
            who_e = who;
            return EINTR;
        }
    }
    return 0;
}

/*===========================================================================*
 *                             processExit                                   *
 *===========================================================================*/

int processExit(message* m){
    int result = -1;
    endpoint_t who = m->PM_PROC;
    for (int i=mutexesHead; i!=mutexesTail; i=(i+1)%MUTEX_NR){
        result = lookThroughQueue(i, who);
        if (result != -1) {
            if (result == mutexQueues[i].head){
                doUnlock(i, m);
                continue;
            }
            for (int k=result; (k+1)%NR_PROCS != mutexQueues[i].tail;){
                mutexQueues[i].T[k] = mutexQueues[i].T[(k+1)%NR_PROCS];
                k++;
                k%=NR_PROCS;
            }
            if (mutexQueues[i].tail == 0)
                mutexQueues[i].tail = NR_PROCS - 1;
            else
                mutexQueues[i].tail--;
            return 0;
        }
    }
    for (int i=cvsHead; i!=cvsTail; i=(i+1)%NR_PROCS){
        result = lookThroughCVQueue(i, who);
        if (result != -1) {
            for (int k=result; (k+1)%NR_PROCS != CVQueues[i].tail;){
                CVQueues[i].T[k] = CVQueues[i].T[(k+1)%NR_PROCS];
                k++;
                k%=NR_PROCS;
            }
            if (CVQueues[i].tail == 0)
                CVQueues[i].tail = NR_PROCS - 1;
            else
                CVQueues[i].tail--;
            if (CVQueues[i].head == CVQueues[i].tail)
                removeCV(i);
            return 0;
        }
    }
    return 0;
}
