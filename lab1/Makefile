# Makefile for mysort application
#
# Name - Atharva Nandanwar
# University of Colorado Boulder
# Concurrent Programming
# Date - 09/03/2020

CC := g++
CFLAGS = -Wall \
		 -Werror

mysort: main.c mergesort.c bucketsort.c
	@$(CC) $(CFLAGS) -o $@ $? -lm -pthread
	@echo "mysort application compiled."

.PHONY: clean

clean:
	@rm -rf mysort
