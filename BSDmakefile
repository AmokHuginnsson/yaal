.MAKEFILEDEPS:

.sinclude ".my_make"

all .DEFAULT:
	@$(MAKE) -f _aux/mk/master.mk $(@)

.my_make:
	@./_aux/guess_make
