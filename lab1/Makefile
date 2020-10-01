# Makefile for mysort application
#
# Name - Atharva Nandanwar
# University of Colorado Boulder
# Concurrent Programming
# Date - 09/03/2020

CC := gcc
CFLAGS = -Wall \
		 -Werror

mysort: main.c mergesort.c quicksort.c
	@$(CC)  -o $@ $? 
	@echo "mysort application compiled."

.PHONY: clean

clean:
	@rm -rf mysort
