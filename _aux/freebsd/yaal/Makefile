# $FreeBSD$
# yaal (R) 2012 Copyright codestation.org all rights reserved.

PORTNAME=      yaal
PORTVERSION=   0.0.30
#PORTREVISION=  0
CATEGORIES=    devel
MASTER_SITES=  http://codestation.org/ https://codestation.org/

MAINTAINER=    amok@codestation.org
COMMENT=       Yet Another Abstraction Layer

BUILD_DEPENDS= autoconf:${PORTSDIR}/devel/autoconf \
	libtool:${PORTSDIR}/devel/libtool \
	gmake:${PORTSDIR}/devel/gmake \
	doxygen:${PORTSDIR}/devel/doxygen
LIB_DEPENDS= xml2:${PORTSDIR}/textproc/libxml2 \
	xslt:${PORTSDIR}/textproc/libxslt

INSTALL_AS_USER=1

pre-clean:
	${RM} -rf $(WRKDIR)/tmp-root

do-fetch:
	cd ../../../ && git pull

do-extract:
	${MKDIR} $(WRKDIR) && ${LN} -s ../../../../ $(WRKSRC)

do-build:
	cd $(WRKSRC) && $(MAKE) PREFIX=/usr/local SYSCONFDIR=/usr/local/etc debug && $(MAKE) PREFIX=/usr/local SYSCONFDIR=/usr/local/etc release && $(MAKE) doc

do-install:
	${MKDIR} $(WRKDIR)/tmp-root && cd $(WRKSRC) && $(MAKE) DESTDIR=$(WRKDIR)/tmp-root install-debug install-release install-doc

.include <bsd.port.mk>
