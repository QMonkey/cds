EXECPATH = bin
OBJPATH = obj
INCLUDEPATH = list tree hashtable
SRCPATH = test
CC = gcc
OPTIONS = -Wall

EXECS = $(EXECPATH)/list_test $(EXECPATH)/tree_test
OBJS = $(OBJPATH)/list.o $(OBJPATH)/list_test.o $(OBJPATH)/rbtree.o $(OBJPATH)/tree_test.o

all: dir build

dir:
	mkdir -p $(OBJPATH)
	mkdir -p $(EXECPATH)

build: $(EXECS)

$(EXECPATH)/list_test: $(OBJPATH)/list.o $(OBJPATH)/list_test.o
	$(CC) -g $^ -o $@

$(EXECPATH)/tree_test: $(OBJPATH)/rbtree.o $(OBJPATH)/tree_test.o
	$(CC) -g $^ -o $@

$(OBJPATH)/list.o: list/list.c
	$(CC) -g -c $< $(foreach path,$(INCLUDEPATH),-I $(path)) -o $@ $(OPTIONS)

$(OBJPATH)/list_test.o: $(SRCPATH)/list_test.c
	$(CC) -g -c $< $(foreach path,$(INCLUDEPATH),-I $(path)) -o $@ $(OPTIONS)

$(OBJPATH)/rbtree.o: tree/rbtree.c
	$(CC) -g -c $< $(foreach path,$(INCLUDEPATH),-I $(path)) -o $@ $(OPTIONS)

$(OBJPATH)/tree_test.o: $(SRCPATH)/tree_test.c
	$(CC) -g -c $< $(foreach path,$(INCLUDEPATH),-I $(path)) -o $@ $(OPTIONS)

clean:
	-rm -rf $(EXECS) $(OBJS)
