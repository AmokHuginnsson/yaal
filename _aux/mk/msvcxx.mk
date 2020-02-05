# a makefile to build using Microsoft Visual Studio.

.PHONY: all debug release install install-debug install-release clean-debug clean-release purge mrproper
.SECONDEXPANSION:

PROJECT_NAME=$(strip $(shell awk -F '=' '/PRJNAME\s*=/{print $$2}' Makefile.mk.in))

ifneq ($(WINDOWS_PREFIX),)
export PREFIX=$(WINDOWS_PREFIX)
endif

CMAKE=$(wildcard /cygdrive/c/Program\ Files\ (x86)/CMake*/bin/cmake.exe)$(wildcard /cygdrive/c/Program\ Files/CMake*/bin/cmake.exe)
CMAKE:=$(if $(CMAKE),$(CMAKE),$(wildcard c:/Program\ Files\ (x86)/CMake*/bin/cmake.exe)$(wildcard c:/Program\ Files/CMake*/bin/cmake.exe))
CPUS=$(shell grep -c processor /proc/cpuinfo)
INPUTS=$(sort $(wildcard */*.cxx) $(wildcard */*.hxx)  $(wildcard */*/*.cxx) $(wildcard */*/*.hxx))
INPUTS:=$(filter-out build/debug/config.hxx build/debug/commit_id.hxx,$(INPUTS))

all: debug

debug: build/debug/stamp

release: build/release/stamp

build/debug/stamp build/release/stamp: $$(subst stamp,$(PROJECT_NAME).sln,$$(@)) $(INPUTS)
	export VS_VER="x`awk '/# Visual Studio /{gsub(/\\r/, "", $$4); print $$4}' $(dir $(@))/$(PROJECT_NAME).sln`" ; \
	if [ "$${VS_VER}" = "x16" -o "$${VS_VER}" = "x14" ] ; then \
		"$(CMAKE)" --build $(dir $(@)) --config $(patsubst build/%/stamp,%,$(@)) ; \
	else \
		echo "Unsupported Visual Studio C++ version ($${VS_VER})!" && false ; \
	fi && \
	test -f build/debug/yaal_hdata-d.dll -o -f build/release/yaal_hdata.dll -o -f build/debug/$(PROJECT_NAME)/1exec.exe -o -f build/release/$(PROJECT_NAME)/1exec.exe && touch $(@) && \
	test -f $(@)

build/debug/$(PROJECT_NAME).sln build/release/$(PROJECT_NAME).sln: ./configure.js
	mkdir -p $(dir $(@)) && \
	cd $(dir $(@)) && \
	cscript ../../configure.js PROJECT_ROOT=../../ BUILD_TYPE=$(patsubst build/%/$(PROJECT_NAME).sln,%,$(@)) $(EXTRA_FLAGS) ; \
	cd - && \
	test -f $(@)

install: install-debug

install-debug install-release: $$(subst install-,,$$(@))
	@export VS_VER="x`awk '/# Visual Studio /{gsub(/\\r/, "", $$4); print $$4}' build/$(subst install-,,$(@))/$(PROJECT_NAME).sln`" ; \
	if [ "$${VS_VER}" = "x16" -o "$${VS_VER}" = "x14" ] ; then \
		"$(CMAKE)" --build build/$(subst install-,,$(@)) --target install --config $(subst install-,,$(@)) ; \
	else \
		echo "Unsupported Visual Studio C++ version ($${VS_VER})!" && false ; \
	fi

clean-debug clean-release: build/$$(subst clean-,,$$(@))/$(PROJECT_NAME).sln
	@export VS_VER="x`awk '/# Visual Studio /{gsub(/\\r/, "", $$4); print $$4}' $(PROJECT_NAME).sln`" ; \
	if [ "$${VS_VER}" = "x16" -o "$${VS_VER}" = "x14" ] ; then \
		"$(CMAKE)" --build build/$(subst clean-,,$(@)) --target clean ; \
	else \
		echo "Unsupported Visual Studio C++ version ($${VS_VER})!" && false ; \
	fi ; \
	/bin/rm -f build/$(subst clean-,,$(@))/stamp

purge mrproper:
	$(MAKE) purge

