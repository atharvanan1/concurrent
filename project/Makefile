# Makefile for project
#
# Name - Atharva Nandanwar
# University of Colorado Boulder
# Concurrent Programming
# Date - 12/01/2020

CC := g++
CFLAGS = -Wall \
		 -Werror

project: main.cpp
	@$(CC) $(CFLAGS) -o $@ $?
	@echo "Project compilation."

.PHONY: clean

clean:
	@rm -rf project
