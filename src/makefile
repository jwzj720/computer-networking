CC = gcc

# Flags
CFLAGS = -Wall -pthread

# Libraries
LIBS = -lpigpiod_if2 -lrt

MULTI_THREAD_SRCS = multithread.c message_app.c hamming.c selection.c read.c send.c build_packet.c

# ROUTER_SRCS = router.c message_app.c hamming.c selection.c read.c send.c build_packet.c 

MULTI_THREAD_TARGET = test_program

# ROUTER_TARGET = router	

all: $(MULTI_THREAD_TARGET) 

$(MULTI_THREAD_TARGET): $(MULTI_THREAD_SRCS)
	$(CC) $(CFLAGS) -o $(MULTI_THREAD_TARGET) $(MULTI_THREAD_SRCS) $(LIBS)
$()
clean:
	rm -f $(MULTI_THREAD_TARGET)
	# rm -f $(ROUTER_TARGET)

.PHONY: all clean
