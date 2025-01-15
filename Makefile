CC = cc
INCLUDE = 
LIBS = 
OBJ = askthing.o question.o a_string.o a_vector.o
HEADERS = askthing.h question.h a_string.h a_vector.h

RELEASE_CFLAGS = -O2 -Wall -Wextra -pedantic -march=native -flto=auto $(INCLUDE) $(LIBS)
DEBUG_CFLAGS = -O0 -g -Wall -Wextra -pedantic -fsanitize=address $(INCLUDE) $(LIBS)
TARBALLFILES = Makefile LICENSE.md README.md settings.def.h $(OBJ) 

TARGET=debug

ifeq ($(TARGET),debug)
	CFLAGS=$(DEBUG_CFLAGS)
else
	CFLAGS=$(RELEASE_CFLAGS)
endif

askthing: setup $(HEADERS) $(OBJ)
	$(CC) $(CFLAGS) -o askthing $(OBJ)

setup: deps settings

settings:
	test -f settings.h || make defaults

deps:

tarball: deps
	mkdir -p askthing
	cp -rv $(TARBALLFILES) askthing/
	tar czvf askthing.tar.gz askthing
	rm -rf askthing

defaults:
	rm -f settings.h
	cp settings.def.h settings.h

clean:
	rm -rf askthing askthing.tar.gz askthing $(OBJ)
	rm -f 3rdparty/include/*

cleanall: clean defaults

.PHONY: clean cleanall
