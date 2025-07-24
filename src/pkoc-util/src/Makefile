PROGS=pkoc-util validate-certificate

CC=clang
CFLAGS=-c -D SIMULATE_PKOC -g -I. -I../include -I/opt/openbadger/include -I/usr/include/PCSC -Wall -Werror
LINK=clang
LDFLAGS=-g -L/opt/openbadger/lib -leac-7816 -leac-encode -leac-crypto_stub -lopenbadger -lpcsclite -ljansson

all:	${PROGS}

clean:
	rm -f core ${PROGS} *.o core

pkoc-util:	pkoc-util.o Makefile \
	  ob-stub.o pkoc-load-certificate.o pkoc-request-certificate.o pkoc-setup.o \
	  pkoc-util-settings.o \
	  simulate-smartcard.o
	${LINK} -o pkoc-util pkoc-util.o ob-stub.o \
	  pkoc-load-certificate.o pkoc-request-certificate.o pkoc-setup.o pkoc-util-settings.o \
	  simulate-smartcard.o ${LDFLAGS}

validate-certificate:	validate-certificate.o Makefile
	${LINK} -o validate-certificate validate-certificate.o ${LDFLAGS} -lcrypto

ob-stub.o:	ob-stub.c
	${CC} ${CFLAGS} ob-stub.c

pkoc-load-certificate.o:	pkoc-load-certificate.c
	${CC} ${CFLAGS} pkoc-load-certificate.c

pkoc-request-certificate.o:	pkoc-request-certificate.c
	${CC} ${CFLAGS} pkoc-request-certificate.c

pkoc-setup.o:	pkoc-setup.c
	${CC} ${CFLAGS} pkoc-setup.c

pkoc-util.o:	pkoc-util.c
	${CC} ${CFLAGS} pkoc-util.c

pkoc-util-settings.o:	pkoc-util-settings.c
	${CC} ${CFLAGS} pkoc-util-settings.c

simulate-smartcard.o:	simulate-smartcard.c
	${CC} ${CFLAGS} simulate-smartcard.c

validate-certificate.o:	validate-certificate.c
	${CC} ${CFLAGS} validate-certificate.c

