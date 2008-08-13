-include .my_make

.PHONY: all nodebug prof cov install dep clean tags stats mrproper purge environment doc

.DEFAULT:
	@$(MAKE) -f Makefile $(@)

all nodebug prof cov install dep clean tags stats mrproper purge environment doc: Makefile
	@$(MAKE) -f Makefile $(@)

Makefile: configure Makefile.in
	@./configure

configure: configure.ac
	@autoreconf

.my_make:
	@./_aux/guess_make

-include local.mk

local.mk:
