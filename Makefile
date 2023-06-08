CC=gcc
CFLAGS=-Wall -pthread -O3
TARGETS=model gendata oneagent
MAIN=
OBJS=agents.o

all : $(TARGETS)

profile : CFLAGS += -pg
profile : $(TARGETS)

model : TARGET=model
model : MAIL=model.c
model : $(OBJS) parameters.h

gendata : TARGET=model
gendata : MAIN=gendata.c
gendata : $(OBJS) parameters.h

oneagent : TARGET=oneagent
oneagent : MAIN=oneagent.c
oneagent : $(OBJS)

$(TARGET) : $(MAIN) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(OBJS)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $<

clean :
	rm $(TARGETS) $(OBJS)
