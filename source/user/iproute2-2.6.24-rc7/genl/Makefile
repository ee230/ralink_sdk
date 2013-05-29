GENLOBJ=genl.o

include ../Config

GENLMODULES :=
GENLMODULES += ctrl.o

GENLOBJ += $(GENLMODULES)

GENLLIB :=

LDFLAGS += -Wl,-export-dynamic
LDLIBS  += -lm -ldl

all: genl

genl: $(GENLOBJ) $(LIBNETLINK) $(LIBUTIL) $(GENLLIB)

romfs: all
	$(ROMFSINST) /bin/genl

clean:
	rm -f $(GENLOBJ) $(GENLLIB) genl
