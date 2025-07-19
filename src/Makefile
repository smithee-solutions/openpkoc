# openpkoc - Make file for source 

# (C)2023 Smithee Solutions LLC

# PCSC is included because their own includes get it wrong

DEFINES = -D GPL_LICENSE_TERMS_ACCEPTED
INCLUDES_FOR_CYCLONE=compiler_port.h cpu_endian.h crypto_config.h date_time.h os_port.h os_port_config.h os_port_posix.h

PROGS=obtest-pkoc

CC=clang
CFLAGS= ${DEFINES} \
  -c -g -I. -I../include \
  -I../include/crypto_cyclone \
  -I/opt/openbadger/include -I/opt/tester/include \
  -I/opt/crypto/cyclone -I/opt/crypto/cyclone/cyclone_crypto \
  -I/usr/include/PCSC -Wall -Werror
#CFLAGS=-c -g -I/opt/tester/include -I/usr/include/PCSC -I../include -I../openbadger -Wall -Werror
LINK=clang
LDFLAGS=-g -L /opt/openbadger/lib -lopenbadger -leac-encode -leac-crypto_cyclone -L /opt/crypto/lib -lcyclone -ljansson -lpcsclite 

LDFLAGS_OPEN=-g -L /opt/openbadger/lib -lopenbadger -leac-encode -leac-crypto_open-gcm -ljansson -lpcsclite 


all:	${PROGS}
	mkdir -p ../opt/osdp-conformance/bin
	cp ${PROGS} ../opt/osdp-conformance/bin

all-open:	obtest-pkoc-open

clean:
	rm -f core *.o ${PROGS}

obtest-pkoc:	Makefile obtest-pkoc.o op-pkoc.o op-pkoc-print.o op-settings.o
	${LINK} -o obtest-pkoc obtest-pkoc.o op-pkoc.o op-pkoc-print.o op-settings.o ${LDFLAGS}

op-pkoc.o:	op-pkoc.c
	${CC} ${CFLAGS} op-pkoc.c

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

