CC=gcc
CFLAGS=-Wall -pthread -O3 -lm
TARGETS=model gendata oneagent reinfection
MAIN=
OBJS=agents.o random.o

all : $(TARGETS)

profile : CFLAGS += -pg
profile : $(TARGETS)

model : TARGET=model
model : MAIN=model.c
model : $(OBJS) parameters.h

gendata : TARGET=gendata
gendata : MAIN=gendata.c
gendata : $(OBJS) parameters.h

oneagent : TARGET=oneagent
oneagent : MAIN=oneagent.c
oneagent : $(OBJS)

reinfection : TARGET=reinfection
reinfection : MAIN=reinfection.c
reinfection : $(OBJS)


$(TARGET) : $(MAIN) $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(OBJS)

%.o : %.c %.h
	$(CC) $(CFLAGS) -c $<

clean :
	rm $(TARGETS) $(OBJS)
