MY_MAKE:sh=if test ! -f .my_make ; then ./_aux/guess_make ; fi && echo .my_make

include	$(MY_MAKE)

all .DEFAULT:
	@$(MAKE) -f _aux/mk/master.mk $(@)

