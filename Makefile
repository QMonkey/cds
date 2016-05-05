EXECPATH = bin
OBJPATH = obj
INCLUDEPATH = List
SRCPATH = test
CC = gcc
OPTIONS = -Wall

EXECS = $(EXECPATH)/list_test
OBJS = $(OBJPATH)/list_test.o

all: dir build

dir:
	mkdir -p $(OBJPATH)
	mkdir -p $(EXECPATH)

build: $(OBJS) $(EXECS)

$(EXECPATH)/list_test: $(OBJPATH)/list_test.o
	$(CC) -g $< -o $@

$(OBJPATH)/list_test.o: $(SRCPATH)/list_test.c
	$(CC) -g -c $< -I $(INCLUDEPATH) -o $@ $(OPTIONS)
