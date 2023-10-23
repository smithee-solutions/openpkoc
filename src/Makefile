# openpkoc - Make file for source 

# (C)2023 Smithee Solutions LLC

# PCSC is included because their own includes get it wrong

PROGS=obtest-pkoc pkoc-osdp

CC=clang
CFLAGS= -D OPENPKOC_VERSION=\"1.23\" \
  -c -g -I../include -I/opt/tester/include -I/usr/include/PCSC -Wall -Werror
#CFLAGS=-c -g -I/opt/tester/include -I/usr/include/PCSC -I../include -I../openbadger -Wall -Werror
LINK=clang
LDFLAGS=-g /opt/tester/lib/libopenbadger.a -ljansson -lpcsclite 

all:	${PROGS}

clean:
	rm -f core *.o ${PROGS}

obtest-pkoc:	Makefile obtest-pkoc.o ob-pkoc.o ob-settings.o
	${LINK} -o obtest-pkoc obtest-pkoc.o ob-pkoc.o ob-settings.o ${LDFLAGS}

ob-pkoc.o:	ob-pkoc.c \
	../include/ob-pkoc.h
	${CC} ${CFLAGS} ob-pkoc.c

ob-pkoc-commandline.o:	ob-pkoc-commandline.c
	${CC} ${CFLAGS} ob-pkoc-commandline.c

ob-settings.o:	ob-settings.c
	${CC} ${CFLAGS} ob-settings.c

obtest-pkoc.o:	obtest-pkoc.c \
	../include/ob-pkoc.h
	${CC} ${CFLAGS} -DALLOCATE_SELECT obtest-pkoc.c

pkoc-osdp:	pkoc-osdp.o ob-pkoc-commandline.o Makefile
	${LINK} -o pkoc-osdp pkoc-osdp.o ob-pkoc-commandline.o ${LDFLAGS}

pkoc-osdp.o:	pkoc-osdp.c
	${CC} ${CFLAGS} pkoc-osdp.c

