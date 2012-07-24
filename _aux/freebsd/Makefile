# $FreeBSD$
# yaal (R) 2012 Copyright codestation.org all rights reserved.
# NATIVE_MAKE

PORTNAME=      yaal
PORTVERSION!=  awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in
#PORTREVISION=  0
CATEGORIES=    devel
MASTER_SITES=  http://codestation.org/ https://codestation.org/

MAINTAINER=    amok@codestation.org
COMMENT=       Yet Another Abstraction Layer
RESTRICTED=1

BUILD_DEPENDS= autoconf:${PORTSDIR}/devel/autoconf \
	libtool:${PORTSDIR}/devel/libtool \
	gmake:${PORTSDIR}/devel/gmake \
	doxygen:${PORTSDIR}/devel/doxygen
LIB_DEPENDS= xml2:${PORTSDIR}/textproc/libxml2 \
	xslt:${PORTSDIR}/textproc/libxslt

INSTALL_AS_USER=yo

WRKDIR=$(.CURDIR)/work
PKG_DBDIR=$(WRKDIR)/pkg
PORT_DBDIR=$(WRKDIR)/port
MY_DESTDIR=$(WRKDIR)/tmp-root
MY_PREFIX=/usr/local
LOCALBASE=$(MY_DESTDIR)/$(MY_PREFIX)
PREFIX=$(LOCALBASE)
BUILD_ENV=PREFIX=$(MY_PREFIX) DESTDIR=$(MY_DESTDIR) SYSCONFDIR=$(MY_PREFIX)/etc
BINOWN=root
SHAREOWN=root

.PHONY: clean all package

pre-clean:
	${RM} -rf $(MY_DESTDIR) pkg-plist $(PORTNAME)-$(PORTVERSION).tbz && \
	cd ../../ && $(MAKE) purge

do-fetch:
	cd ../../ && git pull

do-extract:
	${MKDIR} $(WRKDIR) && ${LN} -s ../../../ $(WRKSRC)

do-build:
	cd $(WRKSRC) && $(MAKE) $(BUILD_ENV) debug && $(MAKE) $(BUILD_ENV) release && $(MAKE) doc

do-install:
	${MKDIR} $(MY_DESTDIR) && cd $(WRKSRC) && $(MAKE) $(BUILD_ENV) install-debug install-release install-doc

post-install:
	${RM} $(MY_DESTDIR)/$(MY_PREFIX)/lib/mkcache

do-package:
	echo "@name $(PORTNAME)-$(PORTVERSION)" > pkg-plist && \
	cat pkg-plist.begin >> pkg-plist && \
	find $(MY_DESTDIR)/$(MY_PREFIX) -type f -o -type l | sed -e 's;$(MY_DESTDIR)/$(MY_PREFIX)/;;' >> pkg-plist && \
	find -d $(MY_DESTDIR)/$(MY_PREFIX)/share/doc/yaal -type d | sed -e 's;$(MY_DESTDIR)/$(MY_PREFIX)/;@dirrm ;' >> pkg-plist && \
	cat pkg-plist.end >> pkg-plist && \
	/usr/sbin/pkg_create -S $(MY_DESTDIR) -p $(MY_PREFIX) -f pkg-plist -c -"$(COMMENT)" -d pkg-descr $(PORTNAME)-$(PORTVERSION).tbz

.include <bsd.port.mk>
