# a makefile to build using Microsoft Visual Studio.

.PHONY: all debug install clean purge mrproper

PROJECT_NAME=$(notdir $(CURDIR))

VCBUILD=$(wildcard /cygdrive/c/Program\ Files\ \(x86\)/Microsoft\ Visual\ Studio\ 9.0/VC/vcpackages/vcbuild.exe)$(wildcard /cygdrive/c/Program\ Files/Microsoft\ Visual\ Studio\ 9.0/VC/vcpackages/vcbuild.exe)
BUILD_ARTIFACT=ZERO_CHECK.dir/Debug/BuildLog.htm

all: debug

debug: $(BUILD_ARTIFACT)

$(BUILD_ARTIFACT): $(PROJECT_NAME).sln
	"$(VCBUILD)" /showenv /M1 $(PROJECT_NAME).sln "Debug|Win32" ; \
	test -f $(@)

$(PROJECT_NAME).sln: ./configure.js ./local.js
	cscript ./configure.js ; \
	test -f $(@)

install: all
	"$(VCBUILD)" /showenv /M1 INSTALL.vcproj "Debug|Win32"

purge clean mrproper:
	$(MAKE) purge

