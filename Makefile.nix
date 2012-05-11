include rules.nix

.PHONY: all

all: build_loader

build_loader
	$(MAKE) -wC loader

clean:
	$(MAKE) -wC loader clean
