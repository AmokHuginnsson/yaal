# Created by: Marcin Konarski <amok@codestation.org>
# yaal (R) 2020 Copyright codestation.org all rights reserved.
# NATIVE_MAKE

VERIFY_DIR=../../yaalrc.in
VERSION=$(shell awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in)
CODENAME=$(shell sw_vers -productVersion | awk -F '.' '{print $$2;exit}' | sed -e 's/^15$$/catalina/')
ifneq ($(BUILD_ID),)
REVISION=$(BUILD_ID)
REVISION_SEPARATOR=-
endif
NAME=yaal
FORMULAE_DIR=homebrew
FORMULAE=./yaal.rb
ARTIFACT_RAW=$(NAME)--$(VERSION)$(REVISION_SEPARATOR)$(REVISION).$(CODENAME).bottle.tar.gz
ARTIFACT=$(subst --,-,$(ARTIFACT_RAW))
SYSTEM=$(shell uname -s | tr A-Z a-z)
SYSTEM_RELEASE=$(shell uname -r | tr A-Z a-z)
ARCH=$(shell uname -m)
BUNDLE_DIR=yaal-bundle/$(SYSTEM)-$(SYSTEM_RELEASE)/$(ARCH)/$(VERSION)
BUNDLE=yaal-$(VERSION)-$(SYSTEM)-$(SYSTEM_RELEASE)-$(ARCH).tar
BUNDLE_WRAP=yaal-bundle.tar

.PHONY: all package bundle clean do-package

all: $(VERIFY_DIR)
	@echo "Explicit target required!" && echo && echo "supported targets: clean package bundle"

$(VERIFY_DIR):
	@echo "You must run this Makefile from yaal/_deploy/darwin directory." && false

$(FORMULAE_DIR)/$(FORMULAE): $(VERIFY_DIR) formulae.rb.in
	@mkdir -p $(FORMULAE_DIR) \
	&& sed -e 's/@version@/$(VERSION)/g' -e 's/@revision@/$(REVISION_SEPARATOR)$(REVISION)/g' formulae.rb.in > $(@)

$(FORMULAE_DIR)/$(ARTIFACT_RAW): $(VERIFY_DIR) $(FORMULAE_DIR)/$(FORMULAE)
	@cd $(FORMULAE_DIR) \
	&& brew install --build-bottle -v $(FORMULAE) \
	&& cd - && cd ../../../tress/ \
	&& env -u DYLD_LIBRARY_PATH -u DYLD_FALLBACK_LIBRARY_PATH -u LIBRARY_PATH \
		CPLUS_INCLUDE_PATH=/usr/local/include C_INCLUDE_PATH=/usr/local/include \
		make purge debug test \
	&& cd - && cd $(FORMULAE_DIR) \
	&& brew bottle --force-core-tap $(FORMULAE)

$(FORMULAE_DIR)/$(ARTIFACT): $(VERIFY_DIR) $(FORMULAE_DIR)/$(ARTIFACT_RAW)
	@cp $(FORMULAE_DIR)/$(ARTIFACT_RAW) $(@)

do-package: $(VERIFY_DIR) $(FORMULAE_DIR)/$(ARTIFACT)

package:
	@$(MAKE) -f homebrew.mk do-package

bundle: $(VERIFY_DIR) $(FORMULAE_DIR)/$(BUNDLE_WRAP)

$(FORMULAE_DIR)/$(BUNDLE_WRAP): $(VERIFY_DIR) $(FORMULAE_DIR)/$(BUNDLE)
	@cd $(FORMULAE_DIR) && tar cf $(BUNDLE_WRAP) $(BUNDLE)

$(FORMULAE_DIR)/$(BUNDLE): $(VERIFY_DIR) $(FORMULAE_DIR)/$(ARTIFACT)
	@mkdir -p $(FORMULAE_DIR)/$(BUNDLE_DIR) \
	&& cp -f $(FORMULAE_DIR)/$(ARTIFACT) $(FORMULAE_DIR)/$(BUNDLE_DIR) \
	&& cd $(FORMULAE_DIR) \
	&& tar cf $(BUNDLE) yaal-bundle

clean: $(VERIFY_DIR) $(FORMULAE_DIR)/$(FORMULAE)
	@cd $(FORMULAE_DIR) \
	&& brew uninstall --zap $(FORMULAE) ;\
	cd - \
	&& /bin/rm -rf $(FORMULAE_DIR)

