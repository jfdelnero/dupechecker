CC=gcc

UNAME := $(shell uname)

CFLAGS=-O3 -Wall

LDFLAGS=

EXEC=dupechecker

all: $(EXEC)

$(EXEC):  dupechecker.o md5.o file_tree_db.o utils.o file_utils.o
	$(CC) -o $@    $^ $(LDFLAGS)

dupechecker.o: dupechecker.c
	$(CC) -o $@ -c $< $(CFLAGS)

md5.o: md5.c
	$(CC) -o $@ -c $< $(CFLAGS)

file_tree_db.o: file_tree_db.c
	$(CC) -o $@ -c $< $(CFLAGS)

utils.o: utils.c
	$(CC) -o $@ -c $< $(CFLAGS)

file_utils.o: file_utils.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)

.PHONY: clean mrproper
