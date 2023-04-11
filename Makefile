CC = clang
LIBS = -lX11 -lX11-xcb -lxcb -lGL
OPT = -O2
FLAGS = -c -W -Wall -ansi -pedantic


all: Context_exe

Context_exe: opgl_context.c
	$(CC) $^ $(LIBS) $(OPT) -o $@ 












#clang -o Context_exe opgl_context.c 
