# a makefile to build using Microsoft Visual Studio.

.PHONY: all install clean purge mrproper

PROJECT_NAME=$(notdir $(CURDIR))

all: $(PROJECT_NAME).sln
	/cygdrive/c/Program\ Files\ \(x86\)/Microsoft\ Visual\ Studio\ 9.0/VC/vcpackages/vcbuild /showenv /M1 $(PROJECT_NAME).sln "Debug|Win32"

install: all
	/cygdrive/c/Program\ Files\ \(x86\)/Microsoft\ Visual\ Studio\ 9.0/VC/vcpackages/vcbuild /showenv /M1 INSTALL.vcproj "Debug|Win32"

purge clean mrproper:
	$(MAKE) purge

$(PROJECT_NAME).sln: ./configure.js ./local.js
	cscript ./configure.js

