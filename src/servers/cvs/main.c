#include "inc.h"

int identifier = 0x1234;
endpoint_t who_e;
int call_type;
endpoint_t SELF_E;
int mutexes[MUTEX_NR];
int conditionVariables[NR_PROCS];
struct queue mutexQueues[MUTEX_NR];
struct CVQueue CVQueues[MUTEX_NR];
int mutexesHead = 0;
int mutexesTail = 0;
int cvsHead = 0;
int cvsTail = 0;

static struct {
	int type;
	int (*func)(message *);
	int reply;	/* whether the reply action is passed through */
} cvs_calls[] = {
	{ CVS_LOCK,	lock,		1 },
	{ CVS_UNLOCK,	unlock, 	0 },
	{ CVS_WAIT,	wait,		1 },
	{ CVS_BROADCAST,broadcast,	0 },
	{ PM_UNPAUSE,	processUnpause,	1 },
	{ PM_DUMPCORE,	processExit,	1 },
	{ PM_EXIT,	processExit,	1 }
};

#define SIZE(a) (sizeof(a)/sizeof(a[0]))

/* SEF functions and variables. */
static void sef_local_startup(void);
static int sef_cb_init_fresh(int type, sef_init_info_t *info);
static void sef_cb_signal_handler(int signo);

int main(int argc, char *argv[])
{
	message m;

	/* SEF local startup. */
	env_setargs(argc, argv);
	sef_local_startup();
	int r;
	m.m_type = CVS_SET;
	if ((r = sendnb(PM_PROC_NR, &m)) != OK)
                printf("CVS send error %d.\n", r);

	while (TRUE) {

		if ((r = sef_receive(ANY, &m)) != OK)
			printf("sef_receive failed %d.\n", r);
		who_e = m.m_source;
		call_type = m.m_type;

		int cvs_number = -1;

		for (int i=0; i<SIZE(cvs_calls); i++){
			if (cvs_calls[i].type == call_type)
				cvs_number = i;
		}

		/* dispatch message */
		if (cvs_number >= 0 && cvs_number < SIZE(cvs_calls)) {
			int result;

			result = cvs_calls[cvs_number].func(&m);

			/*
			 * The handler of the CVS call did not
			 * post a reply.
			 */
			if (!cvs_calls[cvs_number].reply || result) {

				m.m_type = result;

				if ((r = sendnb(who_e, &m)) != OK)
					printf("CVS send error %d.\n", r);
			}
		} else {
			/* warn and then ignore */
			printf("CVS unknown call type: %d from %d.\n",
				call_type, who_e);
		}
	}

	/* no way to get here */
	return -1;
}

/*===========================================================================*
 *			       sef_local_startup			     *
 *===========================================================================*/
static void sef_local_startup()
{
  /* Register init callbacks. */
  sef_setcb_init_fresh(sef_cb_init_fresh);
  sef_setcb_init_restart(sef_cb_init_fresh);

  /* No live update support for now. */

  /* Register signal callbacks. */
  sef_setcb_signal_handler(sef_cb_signal_handler);

  /* Let SEF perform startup. */
  sef_startup();
}

/*===========================================================================*
 *		            sef_cb_init_fresh                                *
 *===========================================================================*/
static int sef_cb_init_fresh(int UNUSED(type), sef_init_info_t *UNUSED(info))
{
/* Initialize the server. */

  SELF_E = getprocnr();

  return(OK);
}

/*===========================================================================*
 *		            sef_cb_signal_handler                            *
 *===========================================================================*/
static void sef_cb_signal_handler(int signo)
{
  /* Only check for termination signal, ignore anything else. */
  if (signo != SIGTERM) return;

}

