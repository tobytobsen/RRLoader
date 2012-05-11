# evtl. die Pfade mit `which <prog>` lokalisieren lassen
CC=/usr/bin/gcc
LD=/usr/local/cross/bin/i586-elf-ld
LS=/bin/ls
MV=/bin/mv
CP=/bin/cp
RM=/bin/rm
NASM=/usr/bin/nasm
MAKE=/usr/bin/make

LDFLAGS=-L/usr/local/cross/lib
CFLAGS=-fpack-struct 			\
		-fleading-underscore 	\
		-c