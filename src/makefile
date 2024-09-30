CC = gcc

# Flags
CFLAGS = -Wall -pthread

# Libraries
LIBS = -lpigpiod_if2 -lrt

<<<<<<< HEAD
SEND_SRCS = one_way_link_send.c send.c text_7bit.c link_send.c

RECEIVE_SRCS = one_way_link_receive.c text_7bit.c read.c link_receive.c

MULTI_THREAD_SRCS = multithread.c link_receive.c text_7bit.c read.c send.c link_send.c

SEND_TARGET = send
RECEIVE_TARGET = receive
MULTI_THREAD_TARGET = multi_thread

all: $(SEND_TARGET) $(RECEIVE_TARGET) $(MULTI_THREAD_TARGET)
=======
SEND_SRCS = link_send.c send.c text_7bit.c

RECEIVE_SRCS = link_receive.c text_7bit.c read.c

SEND_TARGET = send
RECEIVE_TARGET = receive

all: $(SEND_TARGET) $(RECEIVE_TARGET)
>>>>>>> c2d5ea376abc7551edae591c69ed932c0ecf0a24

$(SEND_TARGET): $(SEND_SRCS)
	$(CC) $(CFLAGS) -o $(SEND_TARGET) $(SEND_SRCS) $(LIBS)

$(RECEIVE_TARGET): $(RECEIVE_SRCS)
	$(CC) $(CFLAGS) -o $(RECEIVE_TARGET) $(RECEIVE_SRCS) $(LIBS)

<<<<<<< HEAD
$(MULTI_THREAD_TARGET): $(MULTI_THREAD_SRCS)
	$(CC) $(CFLAGS) -o $(MULTI_THREAD_TARGET) $(MULTI_THREAD_SRCS) $(LIBS)
$()
clean:
	rm -f $(SEND_TARGET) $(RECEIVE_TARGET) $(MULTI_THREAD_TARGET)

.PHONY: all clean
=======
clean:
	rm -f $(SEND_TARGET) $(RECEIVE_TARGET)

.PHONY: all clean
>>>>>>> c2d5ea376abc7551edae591c69ed932c0ecf0a24
