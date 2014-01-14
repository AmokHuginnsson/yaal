.MAKEFILEDEPS:

.sinclude ".my_make"

.PHONY: doc

default .DEFAULT doc:
	@$(MAKE) -f _aux/mk/master.mk $(@)

.my_make:
	@./_aux/guess_make

