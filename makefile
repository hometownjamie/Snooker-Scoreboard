# compiler
CC := gcc

# debug
DEBUG := -ggdb

# optimisation
OPT := -O0

# warnings
WARN := -Wall -Wno-deprecated-declarations

PTHREAD := -pthread

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

# linker
LD=gcc
LDFLAGS= $(PTHREAD) $(GTKLIB) -lm -export-dynamic

ssb: main.o playerlist.o defaults.o matchplayers.o play.o about.o
		$(LD) -o ssb main.o playerlist.o defaults.o matchplayers.o play.o about.o $(LDFLAGS)

main.o:	main.c ssb.h
		$(CC) -c $(CCFLAGS) main.c $(GTKLIB) -o main.o

playerlist.o: playerlist.c ssb.h
		$(CC) -c $(CCFLAGS) playerlist.c $(GTKLIB) -o playerlist.o

defaults.o:	defaults.c ssb.h
		$(CC) -c $(CCFLAGS) defaults.c $(GTKLIB) -o defaults.o

matchplayers.o:	matchplayers.c ssb.h
		$(CC) -c $(CCFLAGS) matchplayers.c $(GTKLIB) -o matchplayers.o

play.o:	play.c ssb.h
		$(CC) -c $(CCFLAGS) play.c $(GTKLIB) -o play.o

about.o: about.c ssb.h
		$(CC) -c $(CCFLAGS) about.c $(GTKLIB) -o about.o

clean:
		rm -f *.o ssc
