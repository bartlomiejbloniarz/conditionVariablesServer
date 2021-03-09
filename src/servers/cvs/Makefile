# Makefile for CVS
PROG=	cvs
SRCS=	main.c cvs.c utils.c queue.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys -lminlib -ltimers -lcompat_minix

MAN=

BINDIR?= /usr/sbin
FILES=cvs.conf
FILESNAME=cvs
FILESDIR= /etc/system.conf.d

.include <minix.service.mk>
