-include .my_make
COMA=,
include _aux/mk/2_term.mk

define PREPARE_MAIN_TARGET
$(1): build/$(1)/Makefile.mk build/$(1)/config.hxx
	@test -t 1 && TERMINAL="TERM" && export TERMINAL ; \
	$$(call invoke,$$(strip $(2) $$(MAKE) -C $$(dir $$(<)) --no-print-directory -f Makefile.mk -e $$(@)))

mrproper-$(1): clean-$(1)
	@$$(if $$(wildcard build/$(1)/Makefile.mk), $$(MAKE) -C build/$(1) -f Makefile.mk -e mrproper && cd build && /bin/rm -rf $(1))

clean-$(1):
	@$$(if $$(wildcard build/$(1)/Makefile.mk), $$(MAKE) -C build/$(1) -f Makefile.mk -e clean)

endef

MAIN_TARGETS=debug release prof cov
OPT_release=DO_RELEASE=1
OPT_prof=DO_COVERAGE=1
OPT_cov=DO_PROFILING=1

DS=d
FIND=find

.PHONY: all bin clean clean-cov clean-debug clean-prof clean-release clean-dep cov debug dep doc install mrproper mrproper-cov mrproper-debug mrproper-prof mrproper-release release prof purge static stats tags
.NOTPARALLEL: build/%/Makefile.mk build/%/config.hxx build/%/yaalrc configure config.hxx.in

.DEFAULT:
	@test -t 1 && TERMINAL="TERM" && export TERMINAL ; \
	$(call invoke,$(MAKE) -C $(dir $(firstword $(foreach T,$(MAIN_TARGETS),$(wildcard ./build/$(T)/Makefile.mk)) ./)) -f $(firstword $(notdir $(foreach T,$(MAIN_TARGETS),$(wildcard ./build/$(T)/Makefile.mk))) ./_aux/empty) $(@))

all: debug

bin dep doc environment install static stats tags: .my_make
	@test -t 1 && TERMINAL="TERM" && export TERMINAL ; \
	$(call invoke,$(MAKE) -C $(dir $(firstword $(foreach T,$(MAIN_TARGETS),$(wildcard ./build/$(T)/Makefile.mk)) ./)) -f $(firstword $(notdir $(foreach T,$(MAIN_TARGETS),$(wildcard ./build/$(T)/Makefile.mk))) ./_aux/empty) $(@))

$(foreach T, $(MAIN_TARGETS), $(eval $(call PREPARE_MAIN_TARGET,$(T),$(OPT_$(T)))))

build/%/Makefile.mk: configure Makefile.mk.in
	@$(eval DIR = $(dir $(@))) $(call invoke,mkdir -p $(DIR) && cd $(DIR) && if test -t 1 -a "x${VERBOSE}" != "xyes" ; then /bin/rm -f $(notdir $(@)) && ../../configure $(CONFIGURE) | awk '{printf "%s\r", "'`tput cr;tput dl1`'"$$0}' ; else ../../configure $(CONFIGURE) ; fi ; test -f $(notdir $(@)) || exit 1 ; touch -c config.hxx Makefile.mk yaalrc)

build/%/config.hxx: configure config.hxx.in
	@$(eval DIR = $(dir $(@))) $(call invoke,mkdir -p $(DIR) && cd $(DIR) && if test -t 1 -a "x${VERBOSE}" != "xyes" ; then /bin/rm -f $(notdir $(@)) && ../../configure $(CONFIGURE) | awk '{printf "%s\r", "'`tput cr;tput dl1`'"$$0}' ; else ../../configure $(CONFIGURE) ; fi ; test -f $(notdir $(@)) || exit 1 ; touch -c config.hxx Makefile.mk yaalrc)

build/%/yaalrc: configure yaalrc.in
	@$(eval DIR = $(dir $(@))) $(call invoke,mkdir -p $(DIR) && cd $(DIR) && if test -t 1 -a "x${VERBOSE}" != "xyes" ; then /bin/rm -f $(notdir $(@)) && ../../configure $(CONFIGURE) | awk '{printf "%s\r", "'`tput cr;tput dl1`'"$$0}' ; else ../../configure $(CONFIGURE) ; fi ; test -f $(notdir $(@)) || exit 1 ; touch -c config.hxx Makefile.mk yaalrc)

configure config.hxx.in: configure.ac _aux/aclib.m4
	@$(call invoke,autoreconf && touch configure config.hxx.in)

mrproper: $(foreach T, $(MAIN_TARGETS), mrproper-$(T))

clean: $(foreach T, $(MAIN_TARGETS), clean-$(T))

purge: mrproper
	@sh -c '. ./_aux/clean-lib.sh && clean .' && \
	/bin/rm -rf autom4te.cache build config.cache config.status \
		configure.lineno configure Makefile.mk config.hxx config.hxx.in \
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
