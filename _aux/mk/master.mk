.PHONY: all bin clean clean-dep cov dep doc environment install mrproper release prof purge static stats tags 

.DEFAULT:
	@$(MAKE) -f Makefile $(@)

all bin clean clean-dep cov dep doc environment install mrproper release prof purge static stats tags: Makefile config.hxx .my_make
	@$(MAKE) -f Makefile $(@)

Makefile: configure Makefile.in
	@./configure && touch config.hxx Makefile yaalrc

config.hxx: configure config.hxx.in
	@./configure && touch config.hxx Makefile yaalrc

yaalrc: configure yaalrc.in
	@./configure && touch config.hxx Makefile yaalrc

configure config.hxx.in: configure.ac _aux/aclib.m4
	@autoreconf && touch configure config.hxx.in

.my_make:
	@./_aux/guess_make

local.mk:
