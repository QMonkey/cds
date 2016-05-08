EXECPATH = bin
OBJPATH = obj
INCLUDEPATH = include list rbtree
SRCPATH = test
CC = gcc
OPTIONS = -Wall

EXECS = $(EXECPATH)/list_test
OBJS = $(OBJPATH)/_list.o $(OBJPATH)/list.o $(OBJPATH)/list_test.o

all: dir build

dir:
	mkdir -p $(OBJPATH)
	mkdir -p $(EXECPATH)

build: $(EXECS)

$(EXECPATH)/list_test: $(OBJPATH)/_list.o
$(EXECPATH)/list_test: $(OBJPATH)/list.o
$(EXECPATH)/list_test: $(OBJPATH)/list_test.o
	$(CC) -g $< -o $@

%.o: %.c
	$(CC) -g -c $< $(foreach path,$(INCLUDEPATH),-I $(path)) -o $@ $(OPTIONS)

#$(OBJPATH)/_list.o: list/_list.c
	#$(CC) -g -c $< $(foreach path,$(INCLUDEPATH),-I $(path)) -o $@ $(OPTIONS)

#$(OBJPATH)/list.o: list/list.c
	#$(CC) -g -c $< $(foreach path,$(INCLUDEPATH),-I $(path)) -o $@ $(OPTIONS)

#$(OBJPATH)/list_test.o: $(SRCPATH)/list_test.c
	#$(CC) -g -c $< $(foreach path,$(INCLUDEPATH),-I $(path)) -o $@ $(OPTIONS)

clean:
	-rm -rf $(EXECS) $(OBJS)
