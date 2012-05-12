include ../rules.nix

TARGET=bin/libnet

SRC=$(shell find . -depth 1 -name '*.[cS]')
OBJ=$(addsuffix .o, $(subst %, %, $(basename $(SRC))))

CFLAGS+= -I./include/ -I../include/

static: $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET).a $(OBJ)

shared:
	@echo Shared Library is not supported yet

test: static
	$(CC) $(CFLAGS) tests/client.c -o tests/client -L"bin" -lnet
	@exec ./tests/client

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET).*

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $^ -o $@