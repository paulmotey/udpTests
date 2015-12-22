SYS := $(shell gcc -dumpmachine) 

ifneq (, $(findstring linux, $(SYS))) 

	#llLIBS = -lsocket -lnsl	# Do linux things
else 
	LIBS = -lwsock32 -lws2_32 -lmswsock 
endif
CC=gcc
CFLAGS = -g 

all: udp-send 

udp-send: udp-send.o 
	@echo $(SYS)
	$(CC) -o udp-send udp-send.o $(LIBS)


udp-send.o: udp-send.c 


clean:
	rm -f udp-send udp-recv udp-send.o udp-recv.o 
