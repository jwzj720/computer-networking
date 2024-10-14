CC = gcc

# Flags
CFLAGS = -Wall -pthread -o

# Libraries
LIBS = -lpigpiod_if2 -lrt 

ROUTER_SRCS = main.c message_app.c hamming.c selection.c read.c send.c build_packet.c 

ROUTER_TARGET = main	

all: $(ROUTER_TARGET)

$(ROUTER_TARGET): $(ROUTER_SRCS)
	$(CC) $(CFLAGS) -o $(ROUTER_TARGET) $(ROUTER_SRCS) $(LIBS)
$()
clean:
	rm -f $(ROUTER_TARGET)

.PHONY: all clean
