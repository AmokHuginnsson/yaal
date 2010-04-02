-include .my_make
COMA=,
include _aux/mk/2_term.mk

define PREPARE_MAIN_TARGET
$(1): build/$(1)/Makefile.mk build/$(1)/config.hxx $$(if $$(wildcard yaalrc.in),build/$(1)/yaalrc)
	@test -t 1 && TERMINAL="TERM" && export TERMINAL ; \
	$$(call invoke,$$(strip $(2) $$(MAKE) -C $$(dir $$(<)) --no-print-directory -f Makefile.mk -e $$(@)))

mrproper-$(1): clean-$(1)
	@$$(if $$(wildcard build/$(1)/Makefile.mk), $$(MAKE) -C build/$(1) -f Makefile.mk -e mrproper && cd build && /bin/rm -rf $(1))

clean-$(1):
	@$$(if $$(wildcard build/$(1)/Makefile.mk), $$(MAKE) -C build/$(1) -f Makefile.mk -e clean)

install-$(1): build/$(1)/Makefile.mk
	@test -t 1 && TERMINAL="TERM" && export TERMINAL ; \
	$$(if $$(wildcard build/$(1)/Makefile.mk), $$(MAKE) -C build/$(1) -f Makefile.mk -e install)

uninstall-$(1): build/$(1)/Makefile.mk
	@test -t 1 && TERMINAL="TERM" && export TERMINAL ; \
	$$(if $$(wildcard build/$(1)/Makefile.mk), $$(MAKE) -C build/$(1) -f Makefile.mk -e uninstall)
endef

define PREPARE_CONFIG_ITEM
build/%/$(1): configure $(1).in
	@$$(eval DIR = $$(dir $$(@))) \
		$$(call invoke,\
		mkdir -p $$(DIR) && cd $$(DIR) && \
		if test -t 1 -a "x$${VERBOSE}" != "xyes" ; then \
			/bin/rm -f $$(notdir $$(@)) && \
			../../configure $$(CONF_$$(*)) $$(if $$(PREFIX),--prefix=$$(PREFIX)) $$(CONFIGURE) | awk -v CL="`tput cr;tput dl1`" '{printf CL"%s\r", $$$$0}' ; \
		else \
			../../configure $$(CONF_$$(*)) $$(if $$(PREFIX),--prefix=$$(PREFIX)) $$(CONFIGURE) ; \
		fi ; test -f $$(notdir $$(@)) || exit 1 ; touch -c config.hxx Makefile.mk yaalrc)
endef

MAIN_TARGETS=debug release prof cov
.DEFAULT_GOAL:=$(firstword $(notdir $(foreach T,$(MAIN_TARGETS),$(if $(wildcard ./build/$(T)/Makefile.mk),$(T))) $(DEFAULT_TARGET) release))
CONFIG_ITEMS=Makefile.mk config.hxx yaalrc
CONF_release=
CONF_debug=--enable-debug
CONF_prof=--enable-profiling
CONF_cov=--enable-coverage

DS=d
FIND=find

.PHONY: all bin clean clean-cov clean-debug clean-prof clean-release clean-dep cov debug dep doc install install-cov install-debug install-prof install-release mrproper mrproper-cov mrproper-debug mrproper-prof mrproper-release release prof purge static stats tags uninstall
.NOTPARALLEL: build/%/Makefile.mk build/%/config.hxx build/%/yaalrc configure config.hxx.in

default: $(.DEFAULT_GOAL)

all: $(MAIN_TARGETS)

install-all: $(foreach T, $(MAIN_TARGETS), install-$(T))
uninstall-all: $(foreach T, $(MAIN_TARGETS), uninstall-$(T))

bin clean dep doc environment install mrproper static stats tags uninstall .DEFAULT: .my_make  build/$(.DEFAULT_GOAL)/Makefile.mk
	@test -t 1 && TERMINAL="TERM" && export TERMINAL ; \
	$(call invoke,$(MAKE) -C $(dir $(firstword $(foreach T,$(MAIN_TARGETS),$(wildcard ./build/$(T)/Makefile.mk)) ./)) -f $(firstword $(notdir $(foreach T,$(MAIN_TARGETS),$(wildcard ./build/$(T)/Makefile.mk))) ./_aux/empty) $(@))

$(foreach T, $(MAIN_TARGETS), $(eval $(call PREPARE_MAIN_TARGET,$(T),$(OPT_$(T)))))
$(foreach T, $(CONFIG_ITEMS), $(eval $(call PREPARE_CONFIG_ITEM,$(T))))

configure config.hxx.in: configure.ac _aux/aclib.m4
	@$(call invoke,libtoolize --force --install > /dev/null 2>&1 || libtoolize --force > /dev/null 2>&1 && automake --add-missing --force-missing > /dev/null 2>&1 ; autoreconf && touch configure config.hxx.in)

mrproper-all: $(foreach T, $(MAIN_TARGETS), mrproper-$(T))

clean-all: $(foreach T, $(MAIN_TARGETS), clean-$(T))

purge: mrproper-all
	@sh -c '. ./_aux/clean-lib.sh && clean .' && \
	if [ -d _aux -a ! -h _aux ] ; then /bin/rm -f _aux/config.guess _aux/config.sub _aux/install-sh _aux/ltmain.sh _aux/missing ; fi && \
	/bin/rm -rf aclocal.m4 autom4te.cache build config.cache config.status \
		configure.lineno configure.scan configure Makefile.mk config.hxx config.hxx.in \
		config.h config.h.in yaalrc config.log dirs.d doc/html \
		CMakeFiles CMakeCache.txt cmake_install.cmake \
		tags GPATH GRTAGS GSYMS GTAGS make.log *.vcproj.* *.vcproj \
		yaal.sln yaal.suo yaal.ncb *.dir debug release *.so && \
	if [ "x${OSTYPE}" != "xcygwin" ] ; then /bin/rm -f Makefile ; fi

clean-dep:
	@$(call invoke,$(FIND) . -name '*.$(DS)' | xargs /bin/rm -f)

.my_make:
	@./_aux/guess_make

local.mk:

-include local.mk
