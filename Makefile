CC = gcc
LIBS = -lX11 -lX11-xcb -lxcb -lGL
OPT = -O2
FLAGS = -c -W -Wall -ansi -pedantic


all: Context_exe OPcontext.o

Context_exe: main.c OPcontext.o
	$(CC) $^ $(LIBS) $(OPT) -o $@ 

OPcontext.o: OPcontext.c
	$(CC) $^ $(LIBS) $(OPT) -c -o $@

