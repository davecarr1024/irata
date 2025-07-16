.PHONY: all configure build test format clean tidy

all:
	$(MAKE) -C irata

configure:
	$(MAKE) -C irata configure

clean:
	$(MAKE) -C irata clean

test:
	$(MAKE) -C irata test

format:
	$(MAKE) -C irata format

tidy:
	$(MAKE) -C irata tidy

coverage:
	$(MAKE) -C irata coverage