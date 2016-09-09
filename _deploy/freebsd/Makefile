# Created by: Marcin Konarski <amok@codestation.org>
# $FreeBSD$
# yaal (R) 2012 Copyright codestation.org all rights reserved.
# NATIVE_MAKE

PORTNAME=      yaal
PORTVERSION!=  awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in
PORTREVISION=  $(BUILD_ID)
CATEGORIES=    devel
MASTER_SITES=  http://codestation.org/ https://codestation.org/

MAINTAINER=    amok@codestation.org
COMMENT=       Yet Another Abstraction Layer
WWW=           http://codestation.org/

LICENSE=codestation
LICENSE_NAME=$(LICENSE)
LICENSE_FILE=../../doc/COPYRIGHT
LICENSE_PERMS=dist-mirror no-dist-sell pkg-mirror no-pkg-sell

LIB_DEPENDS= \
	libxml2.so:${PORTSDIR}/textproc/libxml2 \
	libxslt.so:${PORTSDIR}/textproc/libxslt
BUILD_DEPENDS= \
	libtool:${PORTSDIR}/devel/libtool \
	doxygen:${PORTSDIR}/devel/doxygen

USES+=gmake
USE_AUTOTOOLS+=autoconf

RESTRICTED=1
NO_CDROM=1
NO_CHECKSUM=1
INSTALL_AS_USER=yo
NEED_ROOT=no

.if ${.MAKE.LEVEL} == 0
PATH := ${PATH}
CC := ${CC}
CXX := ${CXX}
C_INCLUDE_PATH := ${C_INCLUDE_PATH}
CPLUS_INCLUDE_PATH := ${CPLUS_INCLUDE_PATH}
LIBRARY_PATH := ${LIBRARY_PATH}
USER := ${USER}
HOME := ${HOME}
TERM := ${TERM}
.unexport-env
.export PATH HOME USER TERM C_INCLUDE_PATH CPLUS_INCLUDE_PATH LIBRARY_PATH
.endif

WRKDIR=$(.CURDIR)/work
PLIST=$(WRKDIR)/pkg-plist
PKG_DBDIR=$(WRKDIR)/pkg
PORT_DBDIR=$(WRKDIR)/port
BUILD_ENV=PREFIX=$(PREFIX) DESTDIR=$(STAGEDIR) SYSCONFDIR=$(PREFIX)/etc LOCALSTATEDIR=$(PREFIX)/var
BINOWN=root
SHAREOWN=root
SYSTEM!= uname -s | tr A-Z a-z

ARTIFACT=$(WRKDIR)/pkg/$(PKGNAME)$(PKG_SUFX)
BUNDLE_DIR=yaal-bundle/$(SYSTEM)-$(OSREL)/$(ARCH)/$(PORTVERSION)
BUNDLE=yaal-$(PORTVERSION)-$(SYSTEM)-$(OSREL)-$(ARCH).tar
BUNDLE_WRAP=yaal-bundle.tar

.PHONY: clean all package

pre-clean:
	@${RM} -rf $(STAGEDIR) $(PLIST) $(WRKDIR)/$(BUNDLE_WRAP) $(WRKDIR)/yaal-bundle $(WRKDIR)/$(BUNDLE) && \
	cd ../../ && $(MAKE) purge

do-fetch:
	@cd ../../ && if git config --local --get remote.origin.url > /dev/null ; then \
		git pull ; \
	fi

do-extract:
	@${MKDIR} $(WRKDIR) && ${LN} -nsf ../../../ $(WRKSRC)

run-autotools:
	@true

do-configure:
	@/bin/rm -f $(WRKSRC)/configure

do-build:
	@cd $(WRKSRC) && $(MAKE) $(BUILD_ENV) debug && $(MAKE) $(BUILD_ENV) release && $(MAKE) doc

pre-install:
	@${MKDIR} $(STAGEDIR) && cd $(WRKSRC) && $(MAKE) $(BUILD_ENV) install-debug install-release install-doc && cd - && \
	${RM} $(STAGEDIR)/$(PREFIX)/lib/mkcache && \
	echo "@name $(PORTNAME)-$(PORTVERSION)" > $(PLIST) && \
	${CAT} pkg-plist.begin >> $(PLIST) && \
	${FIND} $(STAGEDIR)/$(PREFIX) '(' -type f -o -type l ')' -perm -0100 | ${SED} -e 's;$(STAGEDIR)/$(PREFIX)/;@(,,0755) ;' >> $(PLIST) && \
	${FIND} $(STAGEDIR)/$(PREFIX) '(' -type f -o -type l ')' '!' -perm +0111 | ${SED} -e 's;$(STAGEDIR)/$(PREFIX)/;@(,,0644) ;' >> $(PLIST) && \
	${FIND} -d $(STAGEDIR)/$(PREFIX)/share/doc/yaal -type d | ${SED} -e 's;$(STAGEDIR)/$(PREFIX)/;@dir ;' >> $(PLIST) && \
	${CAT} pkg-plist.end >> $(PLIST)

do-install:
	@cd $(WRKSRC) && $(MAKE) $(BUILD_ENV) DEFAULT_TARGET=debug LC_CTYPE=pl_PL.ISO8859-2 test

.include <bsd.port.mk>

$(ARTIFACT): $(PACKAGE_COOKIE)

$(WRKDIR)/$(BUNDLE): $(ARTIFACT)
	@cd $(WRKDIR) && \
	/bin/rm -rf yaal-bundle && \
	mkdir -p $(BUNDLE_DIR) && \
	cp $(ARTIFACT) $(BUNDLE_DIR) && \
	tar cf $(BUNDLE) yaal-bundle

$(WRKDIR)/$(BUNDLE_WRAP): $(WRKDIR)/$(BUNDLE)
	@cd $(WRKDIR) && \
	tar cf $(BUNDLE_WRAP) $(BUNDLE)

bundle: $(WRKDIR)/$(BUNDLE_WRAP)
