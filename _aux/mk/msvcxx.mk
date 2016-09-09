# a makefile to build using Microsoft Visual Studio.

.PHONY: all debug install clean purge mrproper

PROJECT_NAME=$(notdir $(CURDIR))

ifneq ($(WINDOWS_PREFIX),)
export PREFIX=$(WINDOWS_PREFIX)
endif

CMAKE=$(wildcard /cygdrive/c/Program\ Files\ (x86)/CMake*/bin/cmake.exe)$(wildcard /cygdrive/c/Program\ Files/CMake*/bin/cmake.exe)
BUILD_ARTIFACT=build.stamp
CPUS=$(shell grep -c processor /proc/cpuinfo)
INPUTS=$(sort $(wildcard */*.cxx) $(wildcard */*.hxx)  $(wildcard */*/*.cxx) $(wildcard */*/*.hxx) $(PROJECT_NAME).sln)
INPUTS:=$(filter-out build/debug/config.hxx build/debug/commit_id.hxx,$(INPUTS))

all: debug

debug: $(BUILD_ARTIFACT)

$(BUILD_ARTIFACT): $(INPUTS)
	@export VS_VER="x`awk '/# Visual Studio /{print $$4}' $(PROJECT_NAME).sln`" ; \
	if [ "$${VS_VER}" = "x14" ] ; then \
		"$(CMAKE)" --build . ; \
	else \
		echo "Unsupported Visual Studio C++ version ($${VS_VER})!" && false ; \
	fi && \
	test -f build/debug/yaal_hdata-d.dll -o -f build/release/yaal_hdata.dll -o -f build/debug/$(PROJECT_NAME)/1exec.exe -o -f build/release/$(PROJECT_NAME)/1exec.exe && touch $(BUILD_ARTIFACT) && \
	test -f $(@)

$(PROJECT_NAME).sln: ./configure.js ./local.js
	cscript ./configure.js ; \
	test -f $(@)

install: all
	@export VS_VER="x`awk '/# Visual Studio /{print $$4}' $(PROJECT_NAME).sln`" ; \
	if [ "$${VS_VER}" = "x14" ] ; then \
		"$(CMAKE)" --build . --target install ; \
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
	/bin/rm -f $(BUILD_ARTIFACT)

purge mrproper:
	$(MAKE) purge
