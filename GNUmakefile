-include .my_make

.PHONY: all nodebug prof cov install dep clean tags stats mrproper purge environment doc

.DEFAULT:
	@$(MAKE) -f Makefile $(@)

all nodebug prof cov install dep clean tags stats mrproper purge environment doc: Makefile
	@$(MAKE) -f Makefile $(@)

Makefile: configure
	@./configure

configure: configure.ac
	@autoconf

.my_make:
	@./_aux/guess_make

