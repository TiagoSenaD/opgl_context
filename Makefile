CC = gcc
LIBS = -lX11 -lGL
OPT = -O2
FLAGS = -c -W -Wall -ansi -pedantic
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
TARGET = context.out

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(CC) $^ $(LIBS) $(CFLAGS) -o $@
	@echo "Compilation done"

%.o: %.c
	@$(CC) $(LIBS) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS) $(TARGET)





