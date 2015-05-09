# Created by: Marcin Konarski <amok@codestation.org>
# yaal (R) 2015 Copyright codestation.org all rights reserved.
# NATIVE_MAKE

VERSION=$(shell awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in)
REVISION=$(BUILD_ID)
PORTFILE=devel/yaal/Portfile

.PHONY: clean package bundle

$(PORTFILE): Portfile.in
	@mkdir -p devel/yaal \
	&& sed -e 's/@version@/$(VERSION)/g' -e 's/@revision@/$(REVISION)/g' $(<) > $(@)

package:
	$(MAKE) do-package

do-package: $(PORTFILE)
	@cd devel/yaal && port lint && port dmg

clean: $(PORTFILE)
	@cd devel/yaal && port clean --all \
	&& cd - \
	&& /bin/rm -rf devel

