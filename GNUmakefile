-include .my_make

MAKE+=--no-print-directory

default .DEFAULT:
	@$(MAKE) -f _aux/mk/master.mk $(@)

.my_make:
	@./_aux/guess_make

