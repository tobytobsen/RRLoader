include ../rules.nix

TARGET=bin/rrl

SRC=$(shell find . -name '*.[cS]')
OBJ=$(addsuffix .o, $(subst %, %, $(basename $(SRC))))

CFLAGS+= -I./include/ -I../include/

all:
	

clean:
	$(RM) $(OBJ)


%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.S
	$(CC) $(CFLAGS) $^ -o $@