# a makefile to build using Microsoft Visual Studio.

.PHONY: all debug install clean purge mrproper

PROJECT_NAME=$(notdir $(CURDIR))

VCBUILD=$(wildcard /cygdrive/c/Program\ Files\ (x86)/Microsoft\ Visual\ Studio\ 9.0/VC/vcpackages/vcbuild.exe)$(wildcard /cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio\ 9.0/VC/vcpackages/vcbuild.exe)
MSBUILD=$(wildcard /cygdrive/c/Windows/Microsoft.NET/Framework/v4*/MSBuild.exe)
BUILD_ARTIFACT=build.stamp

all: debug

debug: $(BUILD_ARTIFACT)

$(BUILD_ARTIFACT): $(wildcard */*.cxx) $(wildcard */*.hxx)  $(wildcard */*/*.cxx) $(wildcard */*/*.hxx) $(PROJECT_NAME).sln
	@export VS_VER="x`awk '/# Visual Studio /{print $$4}' $(PROJECT_NAME).sln`" ; \
	if [ "$${VS_VER}" = "x2008" ] ; then \
		"$(VCBUILD)" /showenv /M1 $(PROJECT_NAME).sln "Debug|Win32" ; \
	else \
		if [ "$${VS_VER}" = "x2010" ] ; then \
			"$(MSBUILD)" $(PROJECT_NAME).sln /nologo /m:4 /t:Build /clp:NoItemAndPropertyList /p:Configuration=Debug /p:Platform=Win32 ; \
		else \
			echo "Cannot guess VS version!" && false ; \
		fi \
	fi && \
	test -f build/debug/hdata-d.dll -o -f build/release/hdata-d.dll -o -f build/debug/$(PROJECT_NAME)/1exec.exe -o -f build/release/$(PROJECT_NAME)/1exec.exe && touch $(BUILD_ARTIFACT) && \
	test -f $(@)

$(PROJECT_NAME).sln: ./configure.js ./local.js
	cscript ./configure.js ; \
	test -f $(@)

install: all
	@export VS_VER="x`awk '/# Visual Studio /{print $$4}' $(PROJECT_NAME).sln`" ; \
	if [ "$${VS_VER}" = "x2008" ] ; then \
		"$(VCBUILD)" /showenv /M1 INSTALL.vcproj "Debug|Win32" ; \
	else \
		if [ "$${VS_VER}" = "x2010" ] ; then \
			"$(MSBUILD)" INSTALL.vc*proj /nologo /m:1 /t:Build /clp:NoItemAndPropertyList /p:Configuration=Debug /p:Platform=Win32 ; \
		else \
			echo "Cannot guess VS version!" && false ; \
		fi \
	fi

clean: $(PROJECT_NAME).sln
	@export VS_VER="x`awk '/# Visual Studio /{print $$4}' $(PROJECT_NAME).sln`" ; \
	if [ "$${VS_VER}" = "x2008" ] ; then \
		"$(VCBUILD)" /showenv /M1 $(PROJECT_NAME).sln /clean "Debug|Win32" ; \
	else \
		if [ "$${VS_VER}" = "x2010" ] ; then \
			"$(MSBUILD)" $(PROJECT_NAME).sln /nologo /t:Clean /v:normal /p:Configuration=Debug /p:Platform=Win32 ; \
		else \
			echo "Cannot guess VS version!" && false ; \
		fi \
	fi ; \
	/bin/rm -f $(BUILD_ARTIFACT)

purge mrproper:
	$(MAKE) purge

