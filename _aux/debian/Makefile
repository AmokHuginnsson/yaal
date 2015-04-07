# NATIVE_MAKE

VERIFY_DIR=../../yaalrc.in
VERSION=$(shell awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in)
PACKAGE_VERSION=$(VERSION)$(BUILD_ID)
ARCH=$(shell dpkg-architecture -qDEB_BUILD_ARCH)
SYSTEM=$(shell lsb_release -si | tr A-Z a-z)
CODENAME=$(shell lsb_release -sc | tr A-Z a-z)
COMMIT=$(shell git rev-parse HEAD)
SYSTEM_RELEASE=$(shell lsb_release -sr)
ARTIFACTS=yaal_$(PACKAGE_VERSION)_$(ARCH).deb \
					yaal-dev_$(PACKAGE_VERSION)_$(ARCH).deb \
					yaal-doc_$(PACKAGE_VERSION)_$(ARCH).deb \
					yaal_$(VERSION).dsc \
					yaal_$(VERSION).tar.gz \
					yaal_$(VERSION)_$(ARCH).changes
BUNDLE_DIR=yaal-bundle/$(SYSTEM)-$(SYSTEM_RELEASE)/$(ARCH)/$(VERSION)
BUNDLE=yaal-$(VERSION)-$(SYSTEM)-$(SYSTEM_RELEASE)-$(ARCH).tar
BUNDLE_WRAP=yaal-bundle.tar
DEBEMAIL=amok@codestation.org
DCH=dch --force-distribution --distribution "$(CODENAME)"
export DEBEMAIL
export PACKAGE_VERSION

.PHONY: all package bundle clean

all: $(VERIFY_DIR)
	@echo "Explicit target required!" && echo && echo "supported targets: clean package bundle"

$(VERIFY_DIR):
	@echo "You must run this Makefile from yaal/_aux/debian directory." && false

changelog: $(VERIFY_DIR)
	@/bin/rm -f changelog && \
	cd ../../ && \
	ln -nsf ./_aux/debian && \
	$(DCH) --create --vendor CodeStation.Org --newversion $(VERSION) --package yaal "Vendor Release." && \
	$(DCH) --append "commit: $(COMMIT)" && \
	dch --release release

$(ARTIFACTS): $(VERIFY_DIR) changelog
	@cd ../../ && \
	ln -nsf ./_aux/debian && \
	dpkg-buildpackage -Zgzip -us -uc -F --changes-option=-Dversion=$(PACKAGE_VERSION) && \
	mv $(addprefix ../,$(ARTIFACTS)) _aux/debian

tmp/$(BUNDLE): $(VERIFY_DIR) $(ARTIFACTS)
	@/bin/rm -rf tmp/yaal-bundle && \
	mkdir -p tmp/$(BUNDLE_DIR) && \
	cp $(ARTIFACTS) tmp/$(BUNDLE_DIR) && \
	cd tmp && tar cf $(BUNDLE) yaal-bundle

tmp/$(BUNDLE_WRAP): $(VERIFY_DIR) tmp/$(BUNDLE)
	@cd tmp && tar cf $(BUNDLE_WRAP) $(BUNDLE)

package: $(VERIFY_DIR) $(ARTIFACTS)

bundle: $(VERIFY_DIR) tmp/$(BUNDLE_WRAP)

clean: $(VERIFY_DIR)
	/bin/rm -f yaal*.deb yaal_*.dsc yaal_*.tar.gz yaal_*.changes $(BUNDLE_WRAP) $(BUNDLE) && \
	/bin/rm -rf changelog yaal-bundle && \
	cd ../../ && \
	ln -nsf ./_aux/debian && \
	dch --create --newversion $(VERSION) --package yaal "Vendor Release." && \
	dpkg-buildpackage -Tclean && \
	/bin/rm debian _aux/debian/changelog

