# NATIVE_MAKE

.NOTPARALLEL:

SHELL     = /bin/sh
UNQUOTE   = $(subst $\",,$(1))
VERIFY_DIR=../../yaalrc.in
OUTPUT_DIR=../../debian
CHANGELOG =$(OUTPUT_DIR)/changelog
VERSION=$(shell awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in)
PACKAGE_VERSION=$(VERSION)$(call UNQUOTE,$(BUILD_ID))
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
ARTIFACTS_TARGET=$(addprefix $(OUTPUT_DIR)/,$(ARTIFACTS))
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

$(CHANGELOG): $(VERIFY_DIR)
	@/bin/rm -f $(CHANGELOG) && \
	cd ../../ && \
	if [ ! -d debian ] ; then cp -a ./_aux/debian . ; fi && \
	$(DCH) --create --vendor CodeStation.Org --newversion $(VERSION) --package yaal "Vendor Release." && \
	$(DCH) --append "commit: $(COMMIT)" && \
	dch --release release

$(ARTIFACTS_TARGET): $(VERIFY_DIR) $(CHANGELOG)
	@cd ../../ && \
	dpkg-buildpackage -Zgzip -us -uc -F --changes-option=-Dversion=$(PACKAGE_VERSION) && \
	mv $(addprefix ../,$(ARTIFACTS)) debian/

$(OUTPUT_DIR)/tmp/$(BUNDLE): $(VERIFY_DIR) $(ARTIFACTS_TARGET)
	@/bin/rm -rf tmp/yaal-bundle && \
	mkdir -p $(OUTPUT_DIR)/tmp/$(BUNDLE_DIR) && \
	cp $(ARTIFACTS_TARGET) $(OUTPUT_DIR)/tmp/$(BUNDLE_DIR) && \
	cd $(OUTPUT_DIR)/tmp && tar cf $(BUNDLE) yaal-bundle

$(OUTPUT_DIR)/tmp/$(BUNDLE_WRAP): $(VERIFY_DIR) $(OUTPUT_DIR)/tmp/$(BUNDLE)
	@cd $(OUTPUT_DIR)/tmp && tar cf $(BUNDLE_WRAP) $(BUNDLE)

package: $(VERIFY_DIR) $(ARTIFACTS_TARGET)

bundle: $(VERIFY_DIR) $(OUTPUT_DIR)/tmp/$(BUNDLE_WRAP)

clean: $(VERIFY_DIR)
	cd ../../ && \
	if [ ! -d debian ] ; then cp -a ./_aux/debian . ; fi && \
	/bin/rm -f debian/changelog && \
	$(DCH) --create --vendor CodeStation.Org --newversion $(VERSION) --package yaal "Vendor Release." && \
	dpkg-buildpackage -Tclean && \
	/bin/rm -r debian

