.EXPORT_ALL_VARIABLES:

PROGRAMS = radvd 

ifeq ($(CONFIG_USER_RADVD_DUMP),y)
PROGRAMS += radvdump 
endif

LEX = flex
YACC = bison -y
YFLAGS += -d
CFLAGS += -Wall -I./
# radvd_LDADD = -lfl

# COMMON_SRC = log.c socket.c recv.c util.c radvd.h defaults.h pathnames.h \
#         includes.h
# radvd_SOURCES = $(COMMON_SRC) radvd.c timer.c send.c process.c interface.c \
#         device-linux.c device-common.c gram.y gram.h scanner.l
# radvdump_SOURCES = $(COMMON_SRC) radvdump.c

radvd_OBJECTS = log.o socket.o recv.o util.o radvd.o timer.o send.o \
	process.o interface.o device-linux.o device-common.o gram.o scanner.o
radvdump_OBJECTS = log.o socket.o recv.o util.o radvdump.o

all: $(PROGRAMS)

scanner.c: scanner.l
	$(LEX) $(AM_LFLAGS) $(LFLAGS) $< && mv lex.yy.c $@

gram.c: gram.y
	$(YACC) $(AM_YFLAGS) $(YFLAGS) $< && mv y.tab.c $*.c
	if test -f y.tab.h; then \
	if cmp -s y.tab.h $*.h; then rm -f y.tab.h; else mv y.tab.h $*.h; fi; \
	else :; fi

radvd: $(radvd_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(radvd_OBJECTS) $(radvd_LDADD) $(LIBS)

radvdump: $(radvdump_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(radvdump_OBJECTS) $(radvdump_LDADD) $(LIBS)

romfs:
	for i in $(PROGRAMS) ; do $(ROMFSINST) /sbin/$$i ; done
	$(ROMFSINST) /bin/radvd.sh

clean:
	rm -f $(PROGRAMS)
	rm -f scanner.c gram.c gram.h
	rm -f *.o core *.core

