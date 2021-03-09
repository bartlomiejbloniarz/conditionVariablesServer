#include <sys/cdefs.h>
#include <lib.h>
#include <unistd.h>
#include <errno.h>
#include <minix/rs.h>
#include <stdio.h>

int cs_lock(int mutex){
    message m;
    m.m_type = CVS_LOCK;
    m.m1_i1 = mutex;
    endpoint_t cvs_pt;

    if (minix_rs_lookup("cvs", &cvs_pt) != 0){
        errno = ENOSYS;
        return -1;
    }
    while (1) {
        if (sendrec(cvs_pt, &m)) {
            return -1;
        }
        if (m.m_type < 0) {
            if (-m.m_type == EINTR){
                m.m_type = CVS_LOCK;
                m.m1_i1 = -1;
                continue;
            }
            errno = -m.m_type;
            return -1;
        }
        return 0;
    }
}
