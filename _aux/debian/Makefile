# NATIVE_MAKE

VERIFY_DIR=../../yaalrc.in
VERSION=$(shell awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in)
ARCH=$(shell dpkg-architecture -qDEB_BUILD_ARCH)
ARTIFACT=../../yaal_$(VERSION)_$(ARCH).deb
DEBIAN_LINK=../../debian
DEBEMAIL=amok@codestation.org
export DEBEMAIL

all: $(VERIFY_DIR)
	@echo "Explicit target required!" && echo && echo "supported targets: clean package"

$(VERIFY_DIR):
	@echo "You must run this Makefile from yaal/_aux/debian directory." && false

$(DEBIAN_LINK): $(VERIFY_DIR)
	cd ../../ && \
	ln -nsf ./_aux/debian

changelog: $(VERIFY_DIR) $(DEBIAN_LINK)
	/bin/rm -f changelog && \
	cd ../../ && \
	dch --create --newversion 0.0.30 --package yaal "Vendor Release." && \
	dch --release release

$(ARTIFACT): $(VERIFY_DIR) changelog
	cd ../../ && \
	dpkg-buildpackage -us -uc

package: $(VERIFY_DIR) $(ARTIFACT)

clean: $(VERIFY_DIR) changelog
	@cd ../../ && \
	dpkg-buildpackage -Tclean && \
	/bin/rm debian _aux/debian/changelog

