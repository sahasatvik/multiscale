CC=gcc
CFLAGS=-Wall -pthread -O3
MAIN=model.c
TARGET=model
OBJS=agents.o
HEADERS=parameters.h

all : $(TARGET)

$(TARGET) : $(MAIN) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(OBJS)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $<

clean :
	rm $(TARGET) $(OBJS)
