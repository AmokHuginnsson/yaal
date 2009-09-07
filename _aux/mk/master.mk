.PHONY: all bin clean clean-dep cov debug dep doc environment install mrproper release prof purge static stats tags 

.DEFAULT:
	@$(MAKE) -f Makefile.mk $(@)

all bin clean clean-dep cov debug dep doc environment install mrproper release prof purge static stats tags: Makefile.mk config.hxx .my_make
	@$(MAKE) -f Makefile.mk $(@)

Makefile.mk: configure Makefile.mk.in
	@./configure && touch config.hxx Makefile.mk

config.hxx: configure config.hxx.in
	@./configure && touch config.hxx Makefile.mk

yaalrc: configure yaalrc.in
	@./configure && touch config.hxx Makefile.mk yaalrc

configure config.hxx.in: configure.ac _aux/aclib.m4
	@autoreconf && touch configure config.hxx.in

.my_make:
	@./_aux/guess_make

local.mk:
