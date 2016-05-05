EXECPATH = bin
OBJPATH = obj
INCLUDEPATH = list
SRCPATH = test
CC = gcc
OPTIONS = -Wall

EXECS = $(EXECPATH)/list_test
OBJS = $(OBJPATH)/list.o $(OBJPATH)/list_test.o

all: dir build

dir:
	mkdir -p $(OBJPATH)
	mkdir -p $(EXECPATH)

build: $(EXECS) $(OBJS)

$(EXECPATH)/list_test: $(OBJPATH)/list.o
$(EXECPATH)/list_test: $(OBJPATH)/list_test.o
	$(CC) -g $< -o $@

$(OBJPATH)/list.o: list/list.c
	$(CC) -g -c $< -I $(INCLUDEPATH) -o $@ $(OPTIONS)

$(OBJPATH)/list_test.o: $(SRCPATH)/list_test.c
	$(CC) -g -c $< -I $(INCLUDEPATH) -o $@ $(OPTIONS)

clean:
	-rm -rf $(EXECS) $(OBJS)
