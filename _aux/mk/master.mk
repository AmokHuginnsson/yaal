.PHONY: all bin clean clean-dep cov dep doc environment install mrproper release prof purge static stats tags 

.DEFAULT:
	@$(MAKE) -f Makefile $(@)

all bin clean clean-dep cov dep doc environment install mrproper release prof purge static stats tags: Makefile config.h .my_make
	@$(MAKE) -f Makefile $(@)

Makefile: configure Makefile.in
	@./configure && touch config.h Makefile yaalrc

config.h: configure config.h.in
	@./configure && touch config.h Makefile yaalrc

yaalrc: configure yaalrc.in
	@./configure && touch config.h Makefile yaalrc

configure config.h.in: configure.ac _aux/aclib.m4
	@autoreconf && touch configure config.h.in

.my_make:
	@./_aux/guess_make

local.mk:
