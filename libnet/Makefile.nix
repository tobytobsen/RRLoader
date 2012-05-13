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
	$(CC) $(CFLAGS) tests/client_sync.c -o tests/client_sync -L"bin" -lnet
	
	$(CC) $(CFLAGS) tests/client_async.c -o tests/client_async -L"bin" -lnet

	$(CC) $(CFLAGS) tests/simple_server_sync.c -o tests/simple_server_sync -L"bin" -lnet
	$(CC) $(CFLAGS) tests/simple_client_sync.c -o tests/simple_client_sync -L"bin" -lnet


	@exec ./tests/client_sync > ./tests/client_sync.log &
	@exec ./tests/client_async > ./tests/client_async.log &
	@exec ./tests/simple_server_sync > ./tests/simple_server_sync.log &
	@exec ./tests/simple_client_sync > ./tests/simple_client_sync.log &

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET).*

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $^ -o $@