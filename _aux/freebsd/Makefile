# $FreeBSD$
# yaal (R) 2012 Copyright codestation.org all rights reserved.

PORTNAME=      yaal
PORTVERSION=   0.0.30
PORTREVISION=  0
CATEGORIES=    devel
MASTER_SITES=  http://codestation.org/ https://codestation.org/

MAINTAINER=    amok@codestation.org
COMMENT=       Yet Another Abstraction Layer

BUILD_DEPENDS= autoconf:${PORTSDIR}/devel/autoconf \
	libtool:${PORTSDIR}/devel/libtool \
	gmake:${PORTSDIR}/devel/gmake \
	doxygen:${PORTSDIR}/devel/doxygen \
	libxml2:${PORTSDIR}/textproc/libxml2 \
	libxslt:${PORTSDIR}/textproc/libxslt

do-fetch:
	cd ../../ && git pull

do-extract:
	mkdir -p $(WRKDIR) && ln -s ../../../ $(WRKDIR)/$(PORTNAME)-$(PORTVERSION)

do-build:
	cd $(WRKSRC) && $(MAKE) PREFIX=/usr/local debug && $(MAKE) PREFIX=/usr/local release && $(MAKE) doc

.include <bsd.port.mk>
