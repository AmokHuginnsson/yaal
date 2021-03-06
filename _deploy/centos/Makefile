# NATIVE_MAKE

SHELL     = /bin/sh
UNQUOTE   = $(subst $\",,$(1))
VERIFY_DIR=../../yaalrc.in
VERSION=$(shell awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in)
PACKAGE_VERSION=$(or $(call UNQUOTE,$(BUILD_ID)),1)
ARCH=$(shell arch)
SYSTEM=$(shell lsb_release -si | tr A-Z a-z)
DIST=$(shell rpm --eval '%{dist}')
TAG=$(VERSION)-$(PACKAGE_VERSION)$(DIST).$(ARCH)
SYSTEM_RELEASE=$(shell lsb_release -sr|awk -F '.' '{print $$1}')
ARTIFACT_DIR=rpm/RPMS/$(ARCH)
ARTIFACTS=$(ARTIFACT_DIR)/yaal-$(TAG).rpm \
					$(ARTIFACT_DIR)/yaal-devel-$(TAG).rpm \
					$(ARTIFACT_DIR)/yaal-doc-$(TAG).rpm
BUNDLE_DIR=yaal-bundle/$(SYSTEM)-$(SYSTEM_RELEASE)/$(ARCH)/$(VERSION)
BUNDLE=yaal-$(VERSION)-$(SYSTEM)-$(SYSTEM_RELEASE)-$(ARCH).tar
BUNDLE_WRAP=yaal-bundle.tar

.PHONY: all package bundle clean

all: $(VERIFY_DIR)
	@echo "Explicit target required!" && echo && echo "supported targets: clean package bundle"

$(VERIFY_DIR):
	@echo "You must run this Makefile from yaal/_deploy/centos directory." && false

$(ARTIFACTS): $(VERIFY_DIR)
	@cd ../../ && \
	rpmbuild -D 'BUILD_ID $(PACKAGE_VERSION)' -D '_topdir $(CURDIR)/rpm' -bb ./_deploy/centos/yaal.spec

rpm/$(BUNDLE): $(VERIFY_DIR) $(ARTIFACTS)
	@/bin/rm -rf rpm/yaal-bundle && \
	mkdir -p rpm/$(BUNDLE_DIR) && \
	cp $(ARTIFACTS) rpm/$(BUNDLE_DIR) && \
	cd rpm && tar cf $(BUNDLE) yaal-bundle

rpm/$(BUNDLE_WRAP): $(VERIFY_DIR) rpm/$(BUNDLE)
	@cd rpm && tar cf $(BUNDLE_WRAP) $(BUNDLE)

package: $(VERIFY_DIR) $(ARTIFACTS)

bundle: $(VERIFY_DIR) rpm/$(BUNDLE_WRAP)

clean: $(VERIFY_DIR)
	@/bin/rm -f $(BUNDLE_WRAP) $(BUNDLE) && \
	/bin/rm -rf rpm yaal-bundle && \
	cd ../../ && \
	make purge

