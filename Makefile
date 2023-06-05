CC=gcc
CFLAGS=-Wall -pthread -O3
PROFILE=
MAIN=model.c
TARGET=model
OBJS=agents.o
HEADERS=parameters.h

all : $(TARGET)

$(TARGET) : $(MAIN) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(PROFILE) -o $(TARGET) $(MAIN) $(OBJS)

%.o : %.c %.h
	$(CC) $(CFLAGS) $(PROFILE) -c $<

clean :
	rm $(TARGET) $(OBJS)

profile : PROFILE=-pg
profile : $(TARGET)
