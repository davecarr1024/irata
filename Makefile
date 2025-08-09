.PHONY: all configure build test format clean tidy

all:
	$(MAKE) -C irata

build:
	$(MAKE) -C irata build

configure:
	$(MAKE) -C irata configure

clean:
	$(MAKE) -C irata clean

test:
	$(MAKE) -C irata test

format:
	$(MAKE) -C irata format
