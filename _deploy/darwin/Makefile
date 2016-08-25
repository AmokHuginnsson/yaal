# Created by: Marcin Konarski <amok@codestation.org>
# yaal (R) 2015 Copyright codestation.org all rights reserved.
# NATIVE_MAKE

VERSION=$(shell awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in)
ifneq ($(BUILD_ID),)
REVISION=$(BUILD_ID)
REVISION_SEPARATOR=-
endif
NAME=yaal
PORTDIR=devel/$(NAME)
PORTFILE=$(PORTDIR)/Portfile
ifeq ($(MAKECMDGOALS),do-bundle)
ARTIFACT_DIR=$(shell port -d -D devel/yaal info 2>&1 | awk '/workpath/{print $$4;exit}')
ARTIFACT=$(ARTIFACT_DIR)/$(NAME)-$(VERSION)$(REVISION_SEPARATOR)$(REVISION).dmg
SYSTEM=$(shell uname -s | tr A-Z a-z)
SYSTEM_RELEASE=$(shell uname -r | tr A-Z a-z)
ARCH=$(shell uname -m)
BUNDLE_DIR=yaal-bundle/$(SYSTEM)-$(SYSTEM_RELEASE)/$(ARCH)/$(VERSION)
BUNDLE=yaal-$(VERSION)-$(SYSTEM)-$(SYSTEM_RELEASE)-$(ARCH).tar
BUNDLE_WRAP=yaal-bundle.tar
endif

.PHONY: clean package do-package bundle do-bundle

$(PORTFILE): Portfile.in
	@mkdir -p devel/yaal \
	&& sed -e 's/@version@/$(VERSION)/g' -e 's/@revision@/$(REVISION)/g' $(<) > $(@)

package:
	$(MAKE) do-package

do-package: $(PORTFILE)
	@port -D $(PORTDIR) lint && port -v -D $(PORTDIR) test && port -v -D $(PORTDIR) dmg

bundle: package
	$(MAKE) do-bundle

ifeq ($(MAKECMDGOALS),do-bundle)
$(ARTIFACT): do-package

do-bundle: $(PORTDIR)/$(BUNDLE_WRAP)

$(PORTDIR)/$(BUNDLE_WRAP): $(PORTDIR)/$(BUNDLE)
	@cd $(PORTDIR) && tar cf $(BUNDLE_WRAP) $(BUNDLE)

$(PORTDIR)/$(BUNDLE): $(ARTIFACT)
	@mkdir -p $(PORTDIR)/$(BUNDLE_DIR) \
	&& cp -f $(ARTIFACT) $(PORTDIR)/$(BUNDLE_DIR) \
	&& cd $(PORTDIR) \
	&& tar cf $(BUNDLE) yaal-bundle
endif

clean: $(PORTFILE)
	@port -D $(PORTDIR) clean --all \
	&& /bin/rm -rf devel

