PROGS=pkoc-cert-util validate-certificate

CC=clang
CFLAGS=-c -g -I. -I../include -I/usr/include/PCSC -Wall -Werror
LINK=clang
LDFLAGS=-g -lpcsclite

all:	${PROGS}

clean:
	rm -f core ${PROGS} *.o core

pkoc-cert-util:	pkoc-cert-util.o Makefile \
	  ob-stub.o
	${LINK} -o pkoc-cert-util pkoc-cert-util.o ob-stub.o ${LDFLAGS}

validate-certificate:	validate-certificate.o Makefile
	${LINK} -o validate-certificate validate-certificate.o ${LDFLAGS} -lcrypto

pkoc-cert-util.o:	pkoc-cert-util.c
	${CC} ${CFLAGS} pkoc-cert-util.c

ob-stub.o:	ob-stub.c
	${CC} ${CFLAGS} ob-stub.c

validate-certificate.o:	validate-certificate.c
	${CC} ${CFLAGS} validate-certificate.c

