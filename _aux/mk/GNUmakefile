-include .my_make

MAKEFLAGS+=--no-print-directory

.NOTPARALLEL:

.PHONY: doc tags

default .DEFAULT doc tags:
	@$(MAKE) -f _aux/mk/master.mk $(@)

.my_make:
	@./_aux/guess_make

