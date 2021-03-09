#ifndef _CV_H_
#define _CV_H_

int cs_lock(int);
int cs_unlock(int);

int cs_wait(int, int);
int cs_broadcast(int);

#endif
