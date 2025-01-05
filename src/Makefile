# openpkoc - Make file for source 

# (C)2023 Smithee Solutions LLC

# PCSC is included because their own includes get it wrong

DEFINES = -D GPL_LICENSE_TERMS_ACCEPTED
INCLUDES_FOR_CYCLONE=compiler_port.h cpu_endian.h crypto_config.h date_time.h os_port.h os_port_config.h os_port_posix.h

PROGS=obtest-pkoc pkoc-osdp

CC=clang
CFLAGS= ${DEFINES} \
  -c -g -I. -I../include \
  -I../include/crypto_cyclone \
  -I/opt/openbadger/include -I/opt/tester/include \
  -I/opt/crypto/cyclone -I/opt/crypto/cyclone/cyclone_crypto \
  -I/usr/include/PCSC -Wall -Werror
#CFLAGS=-c -g -I/opt/tester/include -I/usr/include/PCSC -I../include -I../openbadger -Wall -Werror
LINK=clang
LDFLAGS=-g -L /opt/openbadger/lib -lopenbadger -ljansson -lpcsclite 

all:	setup_cyclone

setup_cyclone:

all:	${PROGS}

clean:
	rm -f core *.o ${PROGS}

obtest-pkoc:	Makefile obtest-pkoc.o ob-pkoc.o ob-settings.o
	${LINK} -o obtest-pkoc obtest-pkoc.o ob-pkoc.o ob-settings.o ${LDFLAGS}

ob-pkoc.o:	ob-pkoc.c \
	../include/ob-pkoc.h ${INCLUDES_FOR_CYCLONE}
	${CC} ${CFLAGS} ob-pkoc.c

ob-pkoc-commandline.o:	ob-pkoc-commandline.c ../include/ob-pkoc.h ${INCLUDES_FOR_CYCLONE}
	${CC} ${CFLAGS} ob-pkoc-commandline.c

ob-settings.o:	ob-settings.c ../include/ob-pkoc.h ${INCLUDES_FOR_CYCLONE}
	${CC} ${CFLAGS} ob-settings.c

obtest-pkoc.o:	obtest-pkoc.c \
	../include/ob-pkoc.h ${INCLUDES_FOR_CYCLONE}
	${CC} ${CFLAGS} -DALLOCATE_SELECT obtest-pkoc.c

pkoc-osdp:	pkoc-osdp.o ob-pkoc-commandline.o Makefile
	${LINK} -o pkoc-osdp pkoc-osdp.o ob-pkoc-commandline.o ${LDFLAGS}

pkoc-osdp.o:	pkoc-osdp.c ../include/ob-pkoc.h ${INCLUDES_FOR_CYCLONE}
	${CC} ${CFLAGS} pkoc-osdp.c

