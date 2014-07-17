# Created by: Marcin Konarski <amok@codestation.org>
# $FreeBSD$
# yaal (R) 2012 Copyright codestation.org all rights reserved.
# NATIVE_MAKE

PORTNAME=      yaal
PORTVERSION!=  awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $$3;DOT="."}END{print VERSION}' ../../Makefile.mk.in
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
PKG_DBDIR=$(WRKDIR)/pkg
PORT_DBDIR=$(WRKDIR)/port
BUILD_ENV=PREFIX=$(PREFIX) DESTDIR=$(STAGEDIR) SYSCONFDIR=$(PREFIX)/etc LOCALSTATEDIR=$(PREFIX)/var
BINOWN=root
SHAREOWN=root

.PHONY: clean all package

pre-clean:
	@${RM} -rf $(STAGEDIR) pkg-plist $(PORTNAME)-$(PORTVERSION).tbz && \
	cd ../../ && $(MAKE) purge

do-fetch:
	@cd ../../ && if git config --local --get remote.origin.url > /dev/null ; then \
		git pull ; \
	fi

do-configure:
	@/bin/rm -f $(WRKSRC)/configure

do-extract:
	@${MKDIR} $(WRKDIR) && ${LN} -nsf ../../../ $(WRKSRC)

do-build:
	@cd $(WRKSRC) && $(MAKE) $(BUILD_ENV) debug && $(MAKE) $(BUILD_ENV) release && $(MAKE) doc

pre-install:
	@${MKDIR} $(STAGEDIR) && cd $(WRKSRC) && $(MAKE) $(BUILD_ENV) install-debug install-release install-doc && cd - && \
	${RM} $(STAGEDIR)/$(PREFIX)/lib/mkcache && \
	echo "@name $(PORTNAME)-$(PORTVERSION)" > pkg-plist && \
	${CAT} pkg-plist.begin >> pkg-plist && \
	${FIND} $(STAGEDIR)/$(PREFIX) -type f -o -type l | ${SED} -e 's;$(STAGEDIR)/$(PREFIX)/;;' >> pkg-plist && \
	${FIND} -d $(MY_DESTDIR)/$(PREFIX)/share/doc/yaal -type d | ${SED} -e 's;$(MY_DESTDIR)/$(PREFIX)/;@dirrm ;' >> pkg-plist && \
	${CAT} pkg-plist.end >> pkg-plist

do-install:
	@true

.include <bsd.port.mk>
