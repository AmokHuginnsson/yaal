.PHONY: all nodebug prof cov install dep clean tags stats mrproper purge environment doc

.DEFAULT:
	@$(MAKE) -f Makefile $(<)

all: Makefile
	@$(MAKE) -f Makefile all

Makefile: configure
	@./configure

configure: configure.ac
	@autoconf
