include rules.nix

.PHONY: libnet loader

libnet:
	$(MAKE) -f libnet/Makefile.nix all

loader:
	$(MAKE) -f loader/Makefile.nix all

clean:
	$(MAKE) -f libnet/Makefile.nix clean
	$(MAKE) -f loader/Makefile.nix clean
