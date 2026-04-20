# openpkoc - Make file for source 

# (C)2023 Smithee Solutions LLC

# PCSC is included because their own includes get it wrong

PROGS=obtest-pkoc ob-pkoc-ccure-splitter

CC=clang
CFLAGS= \
  -c -g -I. -I../include \
  -I/opt/openbadger/include -I/opt/tester/include \
  -I/usr/include/PCSC -Wall -Werror
LINK=clang
LDFLAGS=-g -L /opt/openbadger/lib -lopenbadger -lopenbadger-crypto-lite -leac-encode -L /opt/crypto/lib -lwolfssl -ljansson -lpcsclite 

LDFLAGS_OPEN=-g -L /opt/openbadger/lib -lopenbadger -leac-encode -leac-crypto_open-gcm -ljansson -lpcsclite 


all:	${PROGS}
	mkdir -p ../opt/tester/bin
	cp ${PROGS} ../opt/tester/bin

all-open:	obtest-pkoc-open

clean:
	rm -f core *.o ${PROGS}

ob-pkoc-ccure-splitter:	ob-pkoc-ccure-splitter.o Makefile
	${LINK} -o ob-pkoc-ccure-splitter ob-pkoc-ccure-splitter.o

obtest-pkoc:	Makefile obtest-pkoc.o op-pkoc.o op-pkoc-print.o op-settings.o
	${LINK} -o obtest-pkoc obtest-pkoc.o op-pkoc.o op-pkoc-print.o op-settings.o ${LDFLAGS}

op-pkoc.o:	op-pkoc.c
	${CC} ${CFLAGS} op-pkoc.c

ob-pkoc-ccure-splitter.o:	ob-pkoc-ccure-splitter.c
	${CC} ${CFLAGS} ob-pkoc-ccure-splitter.c

ob-pkoc-commandline.o:	ob-pkoc-commandline.c
	${CC} ${CFLAGS} ob-pkoc-commandline.c

obtest-pkoc.o:	obtest-pkoc.c
	${CC} ${CFLAGS} -DALLOCATE_SELECT obtest-pkoc.c

op-pkoc-print.o:	op-pkoc-print.c
	${CC} ${CFLAGS} op-pkoc-print.c

op-settings.o:	op-settings.c
	${CC} ${CFLAGS} op-settings.c

pkoc-osdp:	pkoc-osdp.o ob-pkoc-commandline.o Makefile
	${LINK} -o pkoc-osdp pkoc-osdp.o ob-pkoc-commandline.o ${LDFLAGS}

pkoc-osdp.o:	pkoc-osdp.c ../include/ob-pkoc.h 
	${CC} ${CFLAGS} pkoc-osdp.c

obtest-pkoc-open:	Makefile obtest-pkoc.o op-pkoc.o op-pkoc-print.o op-settings.o
	${LINK} -o obtest-pkoc obtest-pkoc.o op-pkoc.o op-pkoc-print.o op-settings.o ${LDFLAGS_OPEN}

test.o:	test.c
	${CC} ${CFLAGS} test.c

