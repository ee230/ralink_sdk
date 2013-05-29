###############################################################
#
# Purpose: Makefile for "UVC Streamer"
# Author.: Tom Stoeveken (TST)
# Version: 0.0
# License: GPL (inherited from luvcview)
#
###############################################################

APP_BINARY=uvc_stream

LFLAGS += -lpthread

OBJECTS=uvc_stream.o color.o utils.o v4l2uvc.o

all: uga_buga

clean:
	@echo "Cleaning up directory."
	rm -f *.a *.o $(APP_BINARY) core *~ log errlog *.avi

# Applications:
uga_buga: $(OBJECTS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJECTS) -o $(APP_BINARY)

romfs:
	$(ROMFSINST) /bin/$(APP_BINARY)
