
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lm

INCLUDES = func.h 
OBJECTS  = func.o app.o
RM = rm -f

all: seq

seq: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o seq
	
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@
	
run:
	./seq	
	
clean:
	$(RM) seq $(OBJECTS)
