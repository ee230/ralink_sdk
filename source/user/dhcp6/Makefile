#
# $Id: Makefile,v 1.6 2007-10-29 03:18:11 winfred Exp $
#

CFLAGS+= -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DYYTEXT_POINTER=1 -DHAVE_LIBCRYPTO=1 -DHAVE_LIBRESOLV=0 -DHAVE_GETADDRINFO=1 -DHAVE_GETNAMEINFO=1 -DHAVE_IF_NAMETOINDEX=1 -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_FCNTL_H=1 -DHAVE_SYS_IOCTL_H=1 -DHAVE_SYS_TIME_H=1 -DHAVE_SYSLOG_H=1 -DHAVE_UNISTD_H=1 -DHAVE_IFADDRS_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_STRUCT_TM_TM_ZONE=1 -DHAVE_TM_ZONE=1 -DGETPGRP_VOID=1 -DSETPGRP_VOID=1 -DRETSIGTYPE=void -DHAVE_MKTIME=1 -DHAVE_SELECT=1 -DHAVE_SOCKET=1 -DHAVE_ANSI_FUNC=1 -DHAVE_STDARG_H=1
LIBOBJS= ifaddrs.o strlcpy.o strlcat.o
LIBS=	-lcrypt
YACC=	bison -y
LEX=	flex
TARGET=
ifdef CONFIG_USER_DHCP6_SERVER
TARGET+= dhcp6s
endif
ifdef CONFIG_USER_DHCP6_CLIENT
TARGET+= dhcp6c
endif
DESTDIR=

CFLAGS+= -DCONF_DH6OPT_DNS_RESOLVERS=25 \
	-DCONF_DH6OPT_DOMAIN_LIST=26 \
	-DCONF_DH6OPT_IA_PD=30 \
	-DCONF_DH6OPT_IAPREFIX=31

COMMONGENSRCS=lease_token.c
CLIENTGENSRCS=client6_parse.c client6_token.c dad_token.c ra_token.c \
		resolv_token.c radvd_token.c
SERVERGENSRCS=server6_parse.c server6_token.c 
CLIENTOBJS=	dhcp6c.o common.o config.o timer.o client6_addr.o \
		hash.o lease.o netlink.o\
	$(CLIENTGENSRCS:%.c=%.o) $(COMMONGENSRCS:%.c=%.o)
SERVOBJS=	dhcp6s.o common.o timer.o hash.o lease.o \
		server6_conf.o server6_addr.o \
	$(SERVERGENSRCS:%.c=%.o) $(COMMONGENSRCS:%.c=%.o)

CLEANFILES=cf.tab.h cp.tab.h sf.tab.h dad_token.c ra_token.c client6_token.c client6_parse.c \
		server6_parse.c server6_token.c lease_token.c resolv_token.c radvd_token.c

all:	$(TARGET) 
dhcp6c:	$(CLIENTOBJS) $(LIBOBJS)
	$(CC) $(LDFLAGS) -o dhcp6c $(CLIENTOBJS) $(LIBOBJS) $(LIBS) 
dhcp6s:	$(SERVOBJS) $(LIBOBJS)
	$(CC) $(LDFLAGS) -o dhcp6s $(SERVOBJS) $(LIBOBJS) $(LIBS) 

dad_token.c: dad_token.l
	$(LEX) -Pifyy dad_token.l
	mv lex.ifyy.c $@

ra_token.c: ra_token.l
	$(LEX) -Prayy ra_token.l
	mv lex.rayy.c $@

resolv_token.c: resolv_token.l
	$(LEX) -Prvyy resolv_token.l
	mv lex.rvyy.c $@

radvd_token.c: radvd_token.l
	$(LEX) -Prdyy radvd_token.l
	mv lex.rdyy.c $@

client6_parse.c cp.tab.h: client6_parse.y
	$(YACC) -d -p cpyy client6_parse.y
	mv y.tab.h cp.tab.h
	mv y.tab.c client6_parse.c

client6_token.c: client6_token.l
	$(LEX) -Pcpyy client6_token.l
	mv lex.cpyy.c $@	

lease_token.c: lease_token.l
	$(LEX) -Plyy lease_token.l
	mv lex.lyy.c $@

server6_parse.c sf.tab.h: server6_parse.y
	$(YACC) -d -p sfyy server6_parse.y
	mv y.tab.h sf.tab.h
	mv y.tab.c server6_parse.c 

server6_token.c: server6_token.l sf.tab.h
	$(LEX) -Psfyy server6_token.l
	mv lex.sfyy.c $@

clean::
	/bin/rm -f *.o dhcp6s dhcp6c $(CLEANFILES) $(GENSRCS)

romfs:
	for i in $(TARGET) ; do \
		$(ROMFSINST) /sbin/$$i ; \
		$(ROMFSINST) $$i.conf /etc_ro/$$i.conf ; \
	done


