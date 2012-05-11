include rules.nix

.PHONY: libnet loader test

libnet:
	$(MAKE) -C libnet -f Makefile.nix static

loader:
	$(MAKE) -C loader -f Makefile.nix all

clean: 
	$(MAKE) -C libnet -f Makefile.nix clean
	$(MAKE) -C loader -f Makefile.nix clean

test:
	