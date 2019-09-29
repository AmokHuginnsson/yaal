VERIFY_DIR=../../yaalrc.in
CANONICAL_PKGFILENAME=yaal-$(VERSION)-solaris$(OS)-$(ARCH).pkg
PKGFILENAME=$(CANONICAL_PKGFILENAME).gz
PREFIX=/usr/local
SYSCONFDIR=/etc
LOCALSTATEDIR=/var
DESTDIR=$(VPATH)/pkg
MAKE_ENV=PREFIX=$(PREFIX) SYSCONFDIR=$(SYSCONFDIR) LOCALSTATEDIR=$(LOCALSTATEDIR) DESTDIR=$(DESTDIR) CONFIGURE="--enable-auto-sanity"
ARTIFACT=$(DESTDIR)/$(PREFIX)/lib/libhdata.so
BUNDLE_DIR=yaal-bundle/solaris-$(OS)/$(ARCH)/$(PORTVERSION)
BUNDLE=yaal-$(VERSION)-solaris-$(OS)-$(ARCH).tar
BUNDLE_WRAP=yaal-bundle.tar
TEST_ARTIFACT=$(DESTDIR)/test-ok

all: $(VERIFY_DIR)
	@echo "Explicit target required!" && echo && echo "supported targets: clean package bundle"

$(VERIFY_DIR):
	@echo "You must run this Makefile from yaal/_deploy/solaris directory." && false

$(ARTIFACT): $(VERIFY_DIR)
	mkdir -p pkg && \
	cd ../../ && $(MAKE) $(MAKE_ENV) debug release && \
	$(MAKE) $(MAKE_ENV) install-debug install-release

$(TEST_ARTIFACT): $(ARTIFACT)
	cd ../../ && $(MAKE) $(MAKE_ENV) test && touch $(@)

$(PKGFILENAME): $(TEST_ARTIFACT) pkginfo.in solaris.mk GNUmakefile makefile
	@echo "i pkginfo" > Prototype && \
	pkgproto pkg= | awk '{print $$1" "$$2" "$$3" "$$4" root root"}' | sed -e 's/ 0700 / 0755 /' >> Prototype && \
	sed -e 's/@VERSION@/$(VERSION)/' -e 's/@ARCH@/$(ARCH)/' pkginfo.in > pkginfo && \
	pkgmk -o -r . -d . -f Prototype && \
	pkgtrans -s ./ $(CANONICAL_PKGFILENAME) yaal && \
	gzip -9 $(CANONICAL_PKGFILENAME) && \
	/bin/rm -rf yaal

package: $(PKGFILENAME)

clean: $(VERIFY_DIR)
	@/bin/rm -rf pkg tmp yaal-*-solaris*-*.pkg{,.gz} pkginfo Prototype && \
	cd ../../ && $(MAKE) purge

tmp/$(BUNDLE): $(PKGFILENAME)
	@mkdir -p tmp && cd tmp && \
	/bin/rm -rf yaal-bundle && \
	mkdir -p $(BUNDLE_DIR) && \
	cp ../$(PKGFILENAME) $(BUNDLE_DIR) && \
	tar cf $(BUNDLE) yaal-bundle

tmp/$(BUNDLE_WRAP): tmp/$(BUNDLE)
	@cd tmp && \
	tar cf $(BUNDLE_WRAP) $(BUNDLE)

bundle: tmp/$(BUNDLE_WRAP)

