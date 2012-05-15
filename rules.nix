# evtl. die Pfade mit `which <prog>` lokalisieren lassen
CC=/usr/bin/gcc
LD=/usr/local/cross/bin/i586-elf-ld
LS=/bin/ls
MV=/bin/mv
CP=/bin/cp
RM=/bin/rm
AR=/usr/bin/ar
NASM=/usr/bin/nasm
MAKE=/usr/bin/make

ARFLAGS=rcs
LDFLAGS=-L/usr/local/cross/lib
CFLAGS=-fleading-underscore 	\
		-std=c99
