# NATIVE_MAKE

VPATH :sh = pwd
VERIFY_DIR=../../yaalrc.in
ARCH :sh = isalist | awk '{print $1}'
OS :sh = uname -r
PKGFILENAME=yaal-$(VERSION)-solaris$(OS)-$(ARCH).pkg

VERSION :sh = awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $3;DOT="."}END{print VERSION}' ../../Makefile.mk.in
PREFIX=/usr/local
SYSCONFDIR=/etc
LOCALSTATEDIR=/var
DESTDIR=$(VPATH)/pkg
MAKE_ENV=PREFIX=$(PREFIX) SYSCONFDIR=$(SYSCONFDIR) LOCALSTATEDIR=$(LOCALSTATEDIR) DESTDIR=$(DESTDIR)
ARTIFACT=$(DESTDIR)/$(PREFIX)/lib/libhdata.so

all: $(VERIFY_DIR)
	@echo "Explicit target required!" && echo && echo "supported targets: clean build package"

$(VERIFY_DIR):
	@echo "You must run this Makefile from yaal/_aux/solaris directory." && false

$(ARTIFACT): $(VERIFY_DIR)
	mkdir -p pkg && \
	cd ../../ && $(MAKE) $(MAKE_ENV) debug release && \
	$(MAKE) $(MAKE_ENV) install-debug install-release

build: $(ARTIFACT)

$(PKGFILENAME): $(ARTIFACT) pkginfo.in Makefile
	@echo "i pkginfo" > Prototype && \
	pkgproto pkg= | awk '{print $$1" "$$2" "$$3" "$$4" root root"}' | sed -e 's/ 0700 / 0755 /' >> Prototype && \
	sed -e 's/@VERSION@/$(VERSION)/' -e 's/@ARCH@/$(ARCH)/' pkginfo.in > pkginfo && \
	pkgmk -o -r . -d . -f Prototype && \
	pkgtrans -s ./ $(PKGFILENAME) yaal && \
	/bin/rm -rf yaal

package: $(PKGFILENAME)

clean: $(VERIFY_DIR)
	@/bin/rm -rf pkg $(PKGFILENAME) pkginfo Prototype && \
	cd ../../ && $(MAKE) purge

