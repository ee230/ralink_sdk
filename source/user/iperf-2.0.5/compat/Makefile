CFLAGS += -DHAVE_CONFIG_H -I../include -I. -I..
CPPFLAGS += -DHAVE_CONFIG_H -I../include -I. -I..

CPP_OBJS= delay.o
C_OBJS= error.o  gettimeofday.o  inet_ntop.o  inet_pton.o  signal.o  snprintf.o  string.o  Thread.o

all: $(CPP_OBJS) $(C_OBJS) libcompat.a

C_OBJS:
	$(CC) $(CFLAGS) -c -o $*.o $*.c

CPP_OBJS:
	$(CXX) $(CPPFLAGS) -c -o $*.o $*.c

libcompat.a:
	$(AR) cru $@ $(C_OBJS) $(CPP_OBJS)

clean:
	rm -f $(CPP_OBJS) $(C_OBJS) $(LIB)


