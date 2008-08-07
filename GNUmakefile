.PHONY: all

.DEFAULT:
	@$(MAKE) -f Makefile $(<)

all: Makefile
	@$(MAKE) -f Makefile all

Makefile: configure
	@./configure

configure: configure.ac
	@autoconf
