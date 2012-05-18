include ../rules.nix

TARGET=bin/libnet

SRC=$(shell find . -depth 1 -name '*.[cS]')
OBJ=$(addsuffix .o, $(subst %, %, $(basename $(SRC))))

CFLAGS+= -I./include/ -I../include/ -I../ext/include -fnested-functions
LDFLAGS+= -lnet -lcrypto -lssl

static: $(OBJ)
	$(AR) $(ARFLAGS) $(TARGET).a $(OBJ) ../ext/lib/darwin-x86_64/libcrypto.a ../ext/lib/darwin-x86_64/libssl.a

shared:
	@echo Shared Library is not supported yet

test: static
	$(CC) $(CFLAGS) $(LDFLAGS) tests/tcp_client_sync.c -o tests/tcp_client_sync.test -L"bin"
	
	$(CC) $(CFLAGS) $(LDFLAGS) tests/tcp_ipv6_client_sync.c -o tests/tcp_ipv6_client_sync.test -L"bin"

	$(CC) $(CFLAGS) $(LDFLAGS) tests/tcp_client_async.c -o tests/tcp_client_async.test -L"bin" -lnet

	$(CC) $(CFLAGS) $(LDFLAGS) tests/tcp_simple_server_sync.c -o tests/tcp_simple_server_sync.test -L"bin"
	$(CC) $(CFLAGS) $(LDFLAGS) tests/tcp_simple_client_sync.c -o tests/tcp_simple_client_sync.test -L"bin"

	$(CC) $(CFLAGS) $(LDFLAGS) tests/tcp_ipv6_simple_server_sync.c -o tests/tcp_ipv6_simple_server_sync.test -L"bin"
	$(CC) $(CFLAGS) $(LDFLAGS) tests/tcp_ipv6_simple_client_sync.c -o tests/tcp_ipv6_simple_client_sync.test -L"bin"

	$(CC) $(CFLAGS) $(LDFLAGS) tests/udp_client_sync.c -o tests/udp_client_sync.test -L"bin" 

	$(CC) $(CFLAGS) $(LDFLAGS) tests/tcp_client_sync_ssl.c -o tests/tcp_client_sync_ssl.test -L"bin"

	$(CC) $(CFLAGS) $(LDFLAGS) tests/url_build.c -o tests/url_build.test -L"bin"

	$(CC) $(CFLAGS) $(LDFLAGS) tests/url_parse.c -o tests/url_parse.test -L"bin"

	$(CC) $(CFLAGS) $(LDFLAGS) tests/simple_http_client.c -o tests/simple_http_client.test -L"bin"


#	@exec ./tests/tcp_client_sync.test > ./tests/tcp_client_sync.log &
#	@exec ./tests/tcp_ipv6_client_sync.test > ./tests/tcp_ipv6_client_sync.log &
#	@exec ./tests/tcp_client_async.test > ./tests/tcp_client_async.log &
#	@exec ./tests/tcp_simple_server_sync.test > ./tests/tcp_simple_server_sync.log &
#	@exec ./tests/tcp_simple_client_sync.test > ./tests/tcp_simple_client_sync.log &
#
#	@exec ./tests/tcp_ipv6_simple_server_sync.test > ./tests/tcp_ipv6_simple_server_sync.log &
#	@exec ./tests/tcp_ipv6_client_sync.test > ./tests/tcp_ipv6_client_sync.log &

clean:
	$(RM) -f $(OBJ)
	$(RM) -f $(TARGET).*
	$(RM) -f tests/*.log
	$(RM) -f tests/*.test

%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $^ -o $@