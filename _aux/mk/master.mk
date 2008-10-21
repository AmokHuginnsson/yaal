.PHONY: all bin clean clean-dep cov dep doc environment install mrproper release prof purge static stats tags 

.DEFAULT:
	@$(MAKE) -f Makefile $(@)

all bin clean clean-dep cov dep doc environment install mrproper release prof purge static stats tags: Makefile config.h .my_make
	@$(MAKE) -f Makefile $(@)

Makefile: configure Makefile.in
	@./configure

config.h: configure config.h.in
	@./configure

yaalrc: configure yaalrc.in
	@./configure

configure config.h.in: configure.ac _aux/aclib.m4
	@autoreconf

.my_make:
	@./_aux/guess_make

local.mk:
