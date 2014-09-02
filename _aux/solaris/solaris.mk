VERIFY_DIR=../../yaalrc.in
PKGFILENAME=yaal-$(VERSION)-solaris$(OS)-$(ARCH).pkg
PREFIX=/usr/local
SYSCONFDIR=/etc
LOCALSTATEDIR=/var
DESTDIR=$(VPATH)/pkg
MAKE_ENV=PREFIX=$(PREFIX) SYSCONFDIR=$(SYSCONFDIR) LOCALSTATEDIR=$(LOCALSTATEDIR) DESTDIR=$(DESTDIR)
ARTIFACT=$(DESTDIR)/$(PREFIX)/lib/libhdata.so
BUNDLE_DIR=yaal-bundle/solaris-$(OS)/$(ARCH)/$(PORTVERSION)
BUNDLE=yaal-$(VERSION)-$(SYSTEM)-$(OS)-$(ARCH).tar
BUNDLE_WRAP=yaal-bundle.tar

all: $(VERIFY_DIR)
	@echo "Explicit target required!" && echo && echo "supported targets: clean build package"

$(VERIFY_DIR):
	@echo "You must run this Makefile from yaal/_aux/solaris directory." && false

$(ARTIFACT): $(VERIFY_DIR)
	mkdir -p pkg && \
	cd ../../ && $(MAKE) $(MAKE_ENV) debug release && \
	$(MAKE) $(MAKE_ENV) install-debug install-release

build: $(ARTIFACT)

$(PKGFILENAME): $(ARTIFACT) pkginfo.in solaris.mk GNUmakefile makefile
	@echo "i pkginfo" > Prototype && \
	pkgproto pkg= | awk '{print $$1" "$$2" "$$3" "$$4" root root"}' | sed -e 's/ 0700 / 0755 /' >> Prototype && \
	sed -e 's/@VERSION@/$(VERSION)/' -e 's/@ARCH@/$(ARCH)/' pkginfo.in > pkginfo && \
	pkgmk -o -r . -d . -f Prototype && \
	pkgtrans -s ./ $(PKGFILENAME) yaal && \
	/bin/rm -rf yaal

package: $(PKGFILENAME)

clean: $(VERIFY_DIR)
	@/bin/rm -rf pkg tmp $(PKGFILENAME) pkginfo Prototype && \
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

