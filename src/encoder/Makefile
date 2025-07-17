PROGS=pkoc-cert-loader

CC=clang
CFLAGS=-c -g -I. -I../include -I/usr/include/PCSC -Wall -Werror
LINK=clang
LDFLAGS=-g -lpcsclite

all:	${PROGS}

clean:
	rm -f core ${PROGS} *.o core

pkoc-cert-loader:	pkoc-cert-loader.o Makefile \
	  ob-stub.o
	${LINK} -o pkoc-cert-loader pkoc-cert-loader.o ob-stub.o ${LDFLAGS}

pkoc-cert-loader.o:	pkoc-cert-loader.c
	${CC} ${CFLAGS} pkoc-cert-loader.c

ob-stub.o:	ob-stub.c
	${CC} ${CFLAGS} ob-stub.c


