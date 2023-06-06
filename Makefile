CC=gcc
CFLAGS=-Wall -pthread -O3
PROFILE=
TARGETS=model gendata
MAIN=
OBJS=agents.o
HEADERS=parameters.h

all : $(TARGETS)

profile : clean
profile : PROFILE=-pg
profile : $(TARGETS)

model : TARGET=model
model : MAIL=model.c
model : $(OBJS)

gendata : TARGET=model
gendata : MAIN=gendata.c
gendata : $(OBJS)

$(TARGET) : $(MAIN) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(PROFILE) -o $(TARGET) $(MAIN) $(OBJS)

%.o : %.c %.h
	$(CC) $(CFLAGS) $(PROFILE) -c $<

clean :
	rm $(TARGETS) $(OBJS)
