include _aux/mk/00_sanity.mk

.NOTPARALLEL:

-include .my_make
,:=,
include _aux/mk/20_term.mk

MAKEFLAGS+=--no-builtin-rules

ifeq ($(NOTPARALLEL),)
THREAD_COUNT=$(shell ./_aux/thread-count)
MAKEFLAGS+=$(shell echo '$(THREAD_COUNT)' | awk '/^[0-9]+$$/{if ( $$1 > 1 ) { print "-j"$$1 } }')
endif

define PREPARE_MAIN_TARGET
$(1): build/$(1)/Makefile.mk build/$(1)/config.hxx $$(if $$(wildcard yaalrc.in),build/$(1)/yaalrc build/$(1)/yaal.pc)
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
			../../configure \
				$$(CONF_$$(*)) \
				$$(if $$(PREFIX),--prefix=$$(PREFIX)) \
				$$(if $$(SYSCONFDIR),--sysconfdir=$$(SYSCONFDIR)) \
				$$(if $$(BINDIR),--bindir=$$(BINDIR)) \
				$$(if $$(SBINDIR),--sbindir=$$(SBINDIR)) \
				$$(if $$(DATADIR),--datadir=$$(DATADIR)) \
				$$(if $$(LIBDIR),--libdir=$$(LIBDIR)) \
				$$(if $$(DOCDIR),--docdir=$$(DOCDIR)) \
				$$(if $$(MANDIR),--mandir=$$(MANDIR)) \
				$$(if $$(INCLUDEDIR),--includedir=$$(INCLUDEDIR)) \
				$$(if $$(LOCALSTATEDIR),--localstatedir=$$(LOCALSTATEDIR)) \
				$$(if $$(SHAREDSTATEDIR),--sharedstatedir=$$(SHAREDSTATEDIR)) \
				$$(CONFIGURE) | tee -a make.log | awk -v CL="`tput cr;tput dl1`" '{printf CL"%s\r", $$$$0}' ; \
		else \
			../../configure \
				$$(CONF_$$(*)) \
				$$(if $$(PREFIX),--prefix=$$(PREFIX)) \
				$$(if $$(SYSCONFDIR),--sysconfdir=$$(SYSCONFDIR)) \
				$$(if $$(BINDIR),--bindir=$$(BINDIR)) \
				$$(if $$(SBINDIR),--sbindir=$$(SBINDIR)) \
				$$(if $$(DATADIR),--datadir=$$(DATADIR)) \
				$$(if $$(LIBDIR),--libdir=$$(LIBDIR)) \
				$$(if $$(DOCDIR),--docdir=$$(DOCDIR)) \
				$$(if $$(MANDIR),--mandir=$$(MANDIR)) \
				$$(if $$(INCLUDEDIR),--includedir=$$(INCLUDEDIR)) \
				$$(if $$(LOCALSTATEDIR),--localstatedir=$$(LOCALSTATEDIR)) \
				$$(if $$(SHAREDSTATEDIR),--sharedstatedir=$$(SHAREDSTATEDIR)) \
				$$(CONFIGURE) | tee -a make.log ; \
		fi ; test -f $$(notdir $$(@)) || exit 1 ; touch -c config.hxx Makefile.mk yaalrc yaal.pc ; true)
endef

MAIN_TARGETS=debug release reldeb relassert prof cov
.DEFAULT_GOAL:=$(firstword $(notdir $(foreach T,$(MAIN_TARGETS),$(if $(wildcard ./build/$(T)/Makefile.mk),$(T))) $(DEFAULT_TARGET) release))
CONFIG_ITEMS=Makefile.mk configure.mk config.hxx yaalrc yaal.pc
CONF_release=
CONF_reldeb=--enable-reldeb
CONF_relassert=--enable-relassert
CONF_debug=--enable-debug
CONF_prof=--enable-profiling
CONF_cov=--enable-coverage

DS=d
FIND=find

.PHONY: all bin check clean clean-all clean-cov clean-debug clean-prof clean-relassert clean-reldeb clean-release clean-dep cov coverage-stats debug dep distclean doc help install install-all install-cov install-debug install-prof install-relassert install-reldeb install-release mrproper mrproper-all mrproper-cov mrproper-debug mrproper-prof mrproper-relassert mrproper-reldeb mrproper-release relassert reldeb release prepare-coverage-baseline prof purge purge-local spell stats tags uninstall

default: $(.DEFAULT_GOAL)

all: $(MAIN_TARGETS)

install-all: $(foreach T, $(MAIN_TARGETS), install-$(T))
uninstall-all: $(foreach T, $(MAIN_TARGETS), uninstall-$(T))

bin check clean coverage-stats dep doc environment install mrproper prepare-coverage-baseline spell stats tags uninstall .DEFAULT: .my_make  build/$(.DEFAULT_GOAL)/Makefile.mk
	@test -t 1 && TERMINAL="TERM" && export TERMINAL ; \
	$(call invoke,$(MAKE) -C $(dir $(firstword $(foreach T,$(MAIN_TARGETS),$(wildcard ./build/$(T)/Makefile.mk)) ./)) -f $(firstword $(notdir $(foreach T,$(MAIN_TARGETS),$(wildcard ./build/$(T)/Makefile.mk))) ./_aux/empty) $(@))

$(foreach T, $(MAIN_TARGETS), $(eval $(call PREPARE_MAIN_TARGET,$(T),$(OPT_$(T)))))
$(foreach T, $(CONFIG_ITEMS), $(eval $(call PREPARE_CONFIG_ITEM,$(T))))

configure config.hxx.in: configure.ac _aux/aclib.m4
	@$(call invoke,(libtoolize --force --install > /dev/null 2>&1 \
			|| libtoolize --force > /dev/null 2>&1 \
			|| glibtoolize --force --install > /dev/null 2>&1) && \
		automake --add-missing --force-missing > /dev/null 2>&1 ; \
		autoreconf --install --symlink && touch configure config.hxx.in)

mrproper-all: $(foreach T, $(MAIN_TARGETS), mrproper-$(T))

clean-all: $(foreach T, $(MAIN_TARGETS), clean-$(T))

purge-local:
	@grep -qs "^purge-local:" Makefile.mk.in && $(MAKE) -f Makefile.mk.in -e purge-local ; \
	true

do-purge: mrproper-all
	@sh -c '. ./_aux/clean-lib.sh && clean .' && \
	if [ -d _aux -a ! -h _aux ] ; then /bin/rm -f _aux/config.guess _aux/config.sub _aux/install-sh _aux/ltmain.sh _aux/missing ; fi && \
	/bin/rm -rf aclocal.m4 autom4te.cache build config.cache config.status \
		configure.lineno configure.scan configure Makefile.mk config.hxx config.hxx.in \
		config.h config.h.in config.log dirs.d doc/html \
		CMakeFiles CMakeCache.txt CPack*Config.cmake cmake_install.cmake install_manifest*.txt \
		tags GPATH GRTAGS GSYMS GTAGS make.log *.vcproj.* *.vcproj *.vcxproj.* *.vcxproj \
		*.sln *.suo *.ncb *.sdf *.dir _UpgradeReport_Files UpgradeLog.XML debug release *.so \
		ipch Win32 *.sln.cache build.stamp* && \
	if [ "x${OSTYPE}" != "xcygwin" ] ; then /bin/rm -f Makefile ; fi

purge: purge-local do-purge

distclean: do-purge

clean-dep:
	@$(call invoke,$(FIND) . -name '*.$(DS)' | xargs /bin/rm -f)

help:
	@echo "make [FLAGS...] TARGETS...\n" \
		"FLAGS:\n" \
		"VERBOSE=yes       - enable verbose output of build process\n" \
		"\n" \
		"TARGETS:\n" \
		"all               - build all targets\n" \
		"bin               - build only binaries\n" \
		"check             - run checks for built target\n" \
		"clean             - clean build artifatcs for default target\n" \
		"clean-all         - clean build artifatcs for all targets\n" \
		"clean-cov         - clean build artifatcs for coverage target\n" \
		"clean-debug       - clean build artifatcs for debug target\n" \
		"clean-prof        - clean build artifatcs for profiling target\n" \
		""
# all bin check clean clean-all clean-cov clean-debug clean-prof clean-relassert clean-reldeb clean-release clean-dep cov coverage-stats debug dep distclean doc help install install-all install-cov install-debug install-prof install-relassert install-reldeb install-release mrproper mrproper-all mrproper-cov mrproper-debug mrproper-prof mrproper-relassert mrproper-reldeb mrproper-release relassert reldeb release prepare-coverage-baseline prof purge stats tags uninstall

.my_make:
	@./_aux/guess-make

local.mk:

-include local.mk

