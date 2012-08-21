# NATIVE_MAKE

VERIFY_DIR=../../yaalrc.in
VERSION=$(shell awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in)
ARCH=$(shell dpkg-architecture -qDEB_BUILD_ARCH)
ARTIFACTS=yaal_$(VERSION)_$(ARCH).deb \
					yaal-dev_$(VERSION)_$(ARCH).deb \
					yaal-doc_$(VERSION)_$(ARCH).deb \
					yaal_$(VERSION).dsc \
					yaal_$(VERSION).tar.gz \
					yaal_$(VERSION)_$(ARCH).changes
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

$(ARTIFACTS): $(VERIFY_DIR) changelog
	cd ../../ && \
	dpkg-buildpackage -us -uc && \
	mv $(addprefix ../,$(ARTIFACTS)) _aux/debian

package: $(VERIFY_DIR) $(ARTIFACTS)

clean: $(VERIFY_DIR) changelog
	@/bin/rm -f $(ARTIFACTS) && \
	cd ../../ && \
	dpkg-buildpackage -Tclean && \
	/bin/rm debian _aux/debian/changelog

