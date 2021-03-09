#include <sys/cdefs.h>
#include <lib.h>
#include <unistd.h>
#include <errno.h>
#include <minix/rs.h>

int cs_broadcast(int cv){
    message m;
    m.m_type = CVS_BROADCAST;
    m.m1_i2 = cv;
    endpoint_t cvs_pt;

    if (minix_rs_lookup("cvs", &cvs_pt) != 0){
        errno = ENOSYS;
        return -1;
    }

    if (sendrec(cvs_pt, &m)){
        return -1;
    }
    if (m.m_type<0){
        errno = -m.m_type;
        return -1;
    }
    return 0;
}
