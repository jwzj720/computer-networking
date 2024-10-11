CC = gcc

# Flags
CFLAGS = -Wall -pthread

# Libraries
LIBS = -lpigpiod_if2 -lrt

MULTI_THREAD_SRCS = multithread.c message_app.c hamming.c selection.c read.c send.c build_packet.c

ROUTER_SRCS = main.c message_app.c hamming.c selection.c read.c send.c build_packet.c 

MULTI_THREAD_TARGET = multi

ROUTER_TARGET = main	

all: $(MULTI_THREAD_TARGET) $(ROUTER_TARGET)

$(MULTI_THREAD_TARGET): $(MULTI_THREAD_SRCS)
	$(CC) $(CFLAGS) -o $(MULTI_THREAD_TARGET) $(MULTI_THREAD_SRCS) $(LIBS)
$(ROUTER_TARGET): $(ROUTER_SRCS)
	$(CC) $(CFLAGS) -o $(ROUTER_TARGET) $(ROUTER_SRCS) $(LIBS)
$()
clean:
	rm -f $(MULTI_THREAD_TARGET)
	rm -f $(ROUTER_TARGET)

.PHONY: all clean
