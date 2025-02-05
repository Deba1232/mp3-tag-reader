# SHELL := /bin/bash
# export PATH := $(PATH):.

OBJ := $(patsubst %.c,%.o,$(wildcard *.c))

mp3tag: $(OBJ)	
	gcc -o $@ $^

%.o: %.c
	gcc -c $< -o $@

clean:
	rm -f *.o mp3tag