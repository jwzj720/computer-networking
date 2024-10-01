CC = gcc

# Flags
CFLAGS = -Wall -pthread

# Libraries
LIBS = -lpigpiod_if2 -lrt

MULTI_THREAD_SRCS = multithread.c transmission.c encoding.c read.c send.c 

MULTI_THREAD_TARGET = multi_thread

all: $(MULTI_THREAD_TARGET)

$(MULTI_THREAD_TARGET): $(MULTI_THREAD_SRCS)
	$(CC) $(CFLAGS) -o $(MULTI_THREAD_TARGET) $(MULTI_THREAD_SRCS) $(LIBS)
$()
clean:
	rm -f $(MULTI_THREAD_TARGET)

.PHONY: all clean
