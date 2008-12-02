MY_MAKE:sh=if test -f .my_make ; then echo .my_make ; else echo ./_aux/empty ; fi
MY_LOCAL:sh=if test -f local.mk ; then echo local.mk ; else echo ./_aux/empty ; fi

include	$(MY_MAKE)
include _aux/mk/master.mk
include $(MY_LOCAL)

