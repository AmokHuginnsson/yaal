# a makefile to build using Microsoft Visual Studio.

.PHONY: all debug release install install-debug install-release clean purge mrproper sln-debug sln-release
.SECONDEXPANSION:

PROJECT_NAME=$(notdir $(CURDIR))

ifneq ($(WINDOWS_PREFIX),)
export PREFIX=$(WINDOWS_PREFIX)
endif

CMAKE=$(wildcard /cygdrive/c/Program\ Files\ (x86)/CMake*/bin/cmake.exe)$(wildcard /cygdrive/c/Program\ Files/CMake*/bin/cmake.exe)
CPUS=$(shell grep -c processor /proc/cpuinfo)
INPUTS=$(sort $(wildcard */*.cxx) $(wildcard */*.hxx)  $(wildcard */*/*.cxx) $(wildcard */*/*.hxx))
INPUTS:=$(filter-out build/debug/config.hxx build/debug/commit_id.hxx,$(INPUTS))

all: debug

debug: build.stamp.debug

release: build.stamp.release

build.stamp.debug build.stamp.release: sln-$$(subst build.stamp.,,$$(@)) $(INPUTS)
	@export VS_VER="x`awk '/# Visual Studio /{print $$4}' $(PROJECT_NAME).sln`" ; \
	if [ "$${VS_VER}" = "x14" ] ; then \
		"$(CMAKE)" --build . --config $(subst build.stamp.,,$(@)) ; \
	else \
		echo "Unsupported Visual Studio C++ version ($${VS_VER})!" && false ; \
	fi && \
	test -f build/debug/yaal_hdata-d.dll -o -f build/release/yaal_hdata.dll -o -f build/debug/$(PROJECT_NAME)/1exec.exe -o -f build/release/$(PROJECT_NAME)/1exec.exe && touch $(@) && \
	test -f $(@)

sln-debug sln-release: ./configure.js
	if [ ! -f $(PROJECT_NAME).sln ] ; then \
		cscript ./configure.js BUILD_TYPE=$(subst .sln,,$(subst sln-,,$(@))) $(EXTRA_FLAGS) ; \
	fi ; \
	test -f $(PROJECT_NAME).sln

install: install-debug

install-debug install-release: $$(subst install-,,$$(@))
	@export VS_VER="x`awk '/# Visual Studio /{print $$4}' $(PROJECT_NAME).sln`" ; \
	if [ "$${VS_VER}" = "x14" ] ; then \
		"$(CMAKE)" --build . --target install --config $(subst install-,,$(@)) ; \
	else \
		echo "Unsupported Visual Studio C++ version ($${VS_VER})!" && false ; \
	fi

clean: $(PROJECT_NAME).sln
	@export VS_VER="x`awk '/# Visual Studio /{print $$4}' $(PROJECT_NAME).sln`" ; \
	if [ "$${VS_VER}" = "x14" ] ; then \
		"$(CMAKE)" --build . --target clean ; \
	else \
		echo "Unsupported Visual Studio C++ version ($${VS_VER})!" && false ; \
	fi ; \
	/bin/rm -f build.stamp*

purge mrproper:
	$(MAKE) purge

