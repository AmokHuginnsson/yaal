# NATIVE_MAKE

VERIFY_DIR=../../yaalrc.in
VERSION=$(shell awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in)
ARCH=$(shell dpkg-architecture -qDEB_BUILD_ARCH)
SYSTEM=$(shell lsb_release -si | tr A-Z a-z)
SYSTEM_RELEASE=$(shell lsb_release -sr)
ARTIFACTS=yaal_$(VERSION)_$(ARCH).deb \
					yaal-dev_$(VERSION)_$(ARCH).deb \
					yaal-doc_$(VERSION)_$(ARCH).deb \
					yaal_$(VERSION).dsc \
					yaal_$(VERSION).tar.gz \
					yaal_$(VERSION)_$(ARCH).changes
BUNDLE_DIR=yaal-bundle/$(SYSTEM)-$(SYSTEM_RELEASE)/$(ARCH)/$(VERSION)
BUNDLE=yaal-$(VERSION)-$(SYSTEM)-$(SYSTEM_RELEASE)-$(ARCH).tar
BUNDLE_WRAP=yaal-bundle.tar
DEBEMAIL=amok@codestation.org
export DEBEMAIL

ifneq ($(word 2,$(MAKECMDGOALS)),)
$(error Only one targed allowed at a time.)
endif

.PHONY: all package clean

all: $(VERIFY_DIR)
	@echo "Explicit target required!" && echo && echo "supported targets: clean package"

$(VERIFY_DIR):
	@echo "You must run this Makefile from yaal/_aux/debian directory." && false

changelog: $(VERIFY_DIR)
	/bin/rm -f changelog && \
	cd ../../ && \
	ln -nsf ./_aux/debian && \
	dch --create --newversion $(VERSION) --package yaal "Vendor Release." && \
	dch --release release

$(ARTIFACTS): $(VERIFY_DIR) changelog
	cd ../../ && \
	dpkg-buildpackage -Zgzip -us -uc && \
	mv $(addprefix ../,$(ARTIFACTS)) _aux/debian

$(BUNDLE): $(VERIFY_DIR) $(ARTIFACTS)
	/bin/rm -rf yaal-bundle && \
	mkdir -p $(BUNDLE_DIR) && \
	cp $(ARTIFACTS) $(BUNDLE_DIR) && \
	tar cf $(@) yaal-bundle

$(BUNDLE_WRAP): $(VERIFY_DIR) $(BUNDLE)
	tar cf $(@) $(BUNDLE)

package: $(VERIFY_DIR) $(ARTIFACTS)

bundle: $(VERIFY_DIR) $(BUNDLE_WRAP)

clean: $(VERIFY_DIR) changelog
	@/bin/rm -f $(ARTIFACTS) $(BUNDLE_WRAP) $(BUNDLE) && \
	/bin/rm -rf yaal-bundle && \
	cd ../../ && \
	dpkg-buildpackage -Tclean && \
	/bin/rm debian _aux/debian/changelog

