CC = gcc

# Flags
CFLAGS = -Wall -pthread

# Libraries
LIBS = -lpigpiod_if2 -lrt

SEND_SRCS = link_send.c send.c text_7bit.c

RECEIVE_SRCS = link_receive.c text_7bit.c read.c

MULTI_THREAD_SRCS = multi_thread.c link_receive.c text_7bit.c read.c send.c

SEND_TARGET = send
RECEIVE_TARGET = receive
MULTI_THREAD_TARGET = multi_thread

all: $(SEND_TARGET) $(RECEIVE_TARGET) $(MULTI_THREAD_TARGET)

$(SEND_TARGET): $(SEND_SRCS)
	$(CC) $(CFLAGS) -o $(SEND_TARGET) $(SEND_SRCS) $(LIBS)

$(RECEIVE_TARGET): $(RECEIVE_SRCS)
	$(CC) $(CFLAGS) -o $(RECEIVE_TARGET) $(RECEIVE_SRCS) $(LIBS)

$(MULTI_THREAD_TARGET): $(MULTI_THREAD_SRCS)
	$(CC) $(CFLAGS) -o $(MULTI_THREAD_TARGET) $(MULTI_THREAD_SRCS) $(LIBS)
$()
clean:
	rm -f $(SEND_TARGET) $(RECEIVE_TARGET) $(MULTI_THREAD_TARGET)

.PHONY: all clean