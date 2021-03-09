#define _POSIX_SOURCE      1	/* tell headers to include POSIX stuff */
#define _MINIX             1	/* tell headers to include MINIX stuff */
#define _SYSTEM            1    /* get OK and negative error codes */

#define MUTEX_NR (1024 + 1)

#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/ipc.h>
#include <minix/endpoint.h>
#include <minix/sysutil.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/syslib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <machine/vm.h>
#include <machine/vmparam.h>
#include <sys/vm.h>

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "queue.h"


void giveMutex(endpoint_t ep, message* m);
int findMutex(int mutex);
int createMutex(int mutex);
void removeMutex(int idx);
int findCV(int cv);
int createCV(int cv);
void removeCV(int idx);

int lookThroughQueue(int idx, endpoint_t ep);
int lookThroughCVQueue(int idx, endpoint_t ep);

int lock(message *);
int unlock(message *);
int wait(message *);
int broadcast(message *);
int processUnpause(message *);
int processExit(message *);

EXTERN int identifier;
EXTERN endpoint_t who_e;
EXTERN int call_type;
EXTERN endpoint_t SELF_E;

EXTERN int mutexes[MUTEX_NR];
EXTERN int conditionVariables[NR_PROCS];

EXTERN struct queue mutexQueues[MUTEX_NR];
EXTERN struct CVQueue CVQueues[MUTEX_NR];

EXTERN int mutexesHead;
EXTERN int mutexesTail;
EXTERN int cvsHead;
EXTERN int cvsTail;
