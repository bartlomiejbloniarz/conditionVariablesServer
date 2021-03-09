#include <sys/cdefs.h>
#include <lib.h>
#include <unistd.h>
#include <errno.h>
#include <minix/rs.h>

int cs_wait(int cv,int mutex){
    message m;
    int mode = 0;
    m.m_type = CVS_WAIT;
    m.m1_i1 = mutex;
    m.m1_i2 = cv;
    m.m1_i3 = mode;
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
                m.m_type = CVS_WAIT;
                if (mode <= 1){
                    mode++;
		    m.m1_i1 = mutex;
                    m.m1_i2 = cv;
                }
                m.m1_i3 = mode;
                continue;
            }
            errno = -m.m_type;
            return -1;
        }
        return 0;
    }
}
