# Makefile for mysort application
#
# Name - Atharva Nandanwar
# University of Colorado Boulder
# Concurrent Programming
# Date - 09/03/2020

CC := gcc
CFLAGS = -Wall \
		 -Werror

mysort: main.c bucketsort.c locks.c barriers.c
	@$(CC) $(CFLAGS) -o $@ $? -lm -pthread
	@echo "mysort application compiled."

counter: counter.c locks.c barriers.c
	@$(CC) -o $@ $? -pthread
	@echo "counter application compiled."

.PHONY: clean

clean:
	@rm -rf mysort counter
