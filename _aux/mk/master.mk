.PHONY: all bin clean clean-dep cov dep doc environment install mrproper release prof purge static stats tags 

.DEFAULT:
	@$(MAKE) -f Makefile $(@)

all bin clean clean-dep cov dep doc environment install mrproper release prof purge static stats tags: Makefile
	@$(MAKE) -f Makefile $(@)

Makefile: configure Makefile.in
	@./configure

configure: configure.ac
	@autoreconf

.my_make:
	@./_aux/guess_make

local.mk:
