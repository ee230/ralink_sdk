EXEC = iperf
CFLAGS += -DHAVE_CONFIG_H -I../include -I. -I..
CPPFLAGS += -DHAVE_CONFIG_H -I../include -I. -I..

CPP_OBJS = Client.o  Launch.o  List.o  Listener.o  main.o  PerfSocket.o  Server.o  Settings.o

C_OBJS = Extractor.o gnu_getopt.o gnu_getopt_long.o Locale.o ReportCSV.o ReportDefault.o Reporter.o service.o SocketAddr.o \
	 sockets.o  stdio.o  tcp_window_size.o


all: $(C_OBJS) $(CPP_OBJS) $(EXEC)

C_OBJS:
	$(CC) $(CFLAGS) -c -o $*.o $*.c

CPP_OBJS:
	$(CXX) $(CPPFLAGS) -c -o $*.o $*.c

iperf:
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $(C_OBJS) $(CPP_OBJS) ../compat/libcompat.a -lpthread

romfs:
	$(ROMFSINST) /bin/iperf

clean:
	rm -f $(C_OBJS) $(CPP_OBJS) $(EXEC)


