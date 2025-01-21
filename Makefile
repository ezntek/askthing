CC = cc
INCLUDE = 
LIBS = 
OBJ = askthing.o question.o a_string.o a_vector.o tui/homescreen.o tui/tui.o tui/favorites.o
HEADERS = question.h a_string.h a_vector.h util.h tui/tui.h

RELEASE_CFLAGS = -O2 -Wall -Wextra -pedantic -march=native -flto=auto $(INCLUDE) $(LIBS)
DEBUG_CFLAGS = -O0 -g -Wall -Wextra -pedantic -fsanitize=address $(INCLUDE) $(LIBS)
TARBALLFILES = Makefile LICENSE.md README.md *.c *.h tui

TARGET=debug

ifeq ($(TARGET),debug)
	CFLAGS=$(DEBUG_CFLAGS)
else
	CFLAGS=$(RELEASE_CFLAGS)
endif

askthing: setup $(HEADERS) $(OBJ)
	$(CC) $(CFLAGS) -o askthing $(OBJ)

setup: deps

deps:

tarball: deps
	mkdir -p askthing
	cp -rv $(TARBALLFILES) askthing/
	tar czf askthing.tar.gz askthing
	rm -rf askthing

defaults:

clean:
	rm -rf askthing askthing.tar.gz askthing $(OBJ)
	rm -f 3rdparty/include/*

cleanall: clean defaults

.PHONY: clean cleanall
