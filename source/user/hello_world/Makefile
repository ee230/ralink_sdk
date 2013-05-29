EXE = hello
OBJS = hello.o

#stdlibc++ known issue: 
#If your program  needs libstdc++, please turn off code optimization
CFLAGS += -O0

all: $(OBJS)
	$(CXX) -o $(EXE) $(OBJS) $(LDFLAGS)

.cpp.o:
	$(CXX) $(CFLAGS) -c $< 

romfs:
	$(ROMFSINST) /bin/$(EXE)

clean:
	@rm -rf $(EXE)  *.o

