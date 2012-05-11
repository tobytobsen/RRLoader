include ../rules.nix

TARGET=bin/libnet

SRC=$(shell find . -name '*.[cS]')
OBJ=$(addsuffix .o, $(subst %, %, $(basename $(SRC))))

CFLAGS+= -I./include/ -I../include/

static: $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET).a $(OBJ)

shared:
	

test:

clean:
	$(RM) $(OBJ)


%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.S
	$(CC) $(CFLAGS) $^ -o $@