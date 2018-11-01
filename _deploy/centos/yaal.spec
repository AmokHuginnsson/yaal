Name:    yaal
Version: %(awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $3;DOT="."}END{print VERSION}' ./Makefile.mk.in)
Release: %{?BUILD_ID}%{!?BUILD_ID:1}%{?dist}
Summary: Yet Another Abstraction Layer - general purpose C++ library.

Group:   System Environment/Libraries
License: Commercial (CC BY-ND-NC 4.0, AFPL)
URL:     https://codestation.org/
Source:  https://codestation.org/repo/yaal.git
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires: make, autoconf, libtool, pcre-devel, libxml2-devel, libxslt-devel, zlib-devel, openssl-devel, ncurses-devel, doxygen, graphviz, rubygem-asciidoctor
Requires: pcre, libxml2, libxslt, openssl, zlib, ncurses

%global __requires_exclude_from %{?__requires_exclude:%__requires_exclude|}^.*(libyaal_(sqlite3|postgresql|mysql|firebird|oracle)_driver|pkgconfig).*$
%global _enable_debug_package 0
%global debug_package %{nil}
%global __os_install_post /usr/lib/rpm/brp-compress %{nil}

%description

%package -n yaal-devel
Summary: Yet Another Abstraction Layer - general purpose C++ library - development files.
%description -n yaal-devel

%package -n yaal-doc
Summary: Yet Another Abstraction Layer - general purpose C++ library - developer documentation.
%description -n yaal-doc

%define flagfilter sed -e 's/-O2\\>//g' -e 's/-g\\>//g' -e 's/-Wall\\>//g' -e 's/-Werror\\>//g' -e 's/-Wformat-security\\>//g' -e 's/-Wformat\\>//g'
%define clearflags export CFLAGS=`echo ${CFLAGS} | %{flagfilter}`;export CXXFLAGS=`echo ${CXXFLAGS} | %{flagfilter}`

%prep
umask 0077
if [ -f "${RPM_SOURCE_DIR}/%{name}-%{version}.tar.gz" ] ; then
%setup -T -c
else
	cd -
	/bin/rm -rf "${RPM_BUILD_DIR}/%{name}-%{version}"
	mkdir "${RPM_BUILD_DIR}/%{name}-%{version}"
	tar cf - --exclude build --exclude _deploy/centos/rpm . | tar -x -C "${RPM_BUILD_DIR}/%{name}-%{version}"
	cd -
fi
cd "%{name}-%{version}"
make purge

%build
umask 0077
%{clearflags}
make %{?_smp_mflags} debug release doc PREFIX=%{_prefix} SYSCONFDIR=%{_sysconfdir} LIBDIR=%{_libdir} DOCDIR=%{_pkgdocdir} MANDIR=%{_mandir} LOCALSTATEDIR=%{_localstatedir} CONFIGURE="--enable-auto-sanity"

%install
rm -rf ${RPM_BUILD_ROOT}
umask 0077
make install-debug install-release install-doc DESTDIR=${RPM_BUILD_ROOT}
rm -f ${RPM_BUILD_ROOT}/%{_libdir}/ld.so.conf ${RPM_BUILD_ROOT}/%{_libdir}/ld.so.cache ${RPM_BUILD_ROOT}/%{_libdir}/mkcache

%check
umask 0077
ln -nsf ../../../../../tress ../
make test

%clean
rm -rf ${RPM_BUILD_ROOT}
umask 0077
make purge

%files
%defattr(-,root,root,-)
%{_libdir}/lib*[a-z]?.so*
%{_sysconfdir}/*
%{_localstatedir}/*
%exclude %{_pkgdocdir}/html
%doc doc/INSTALL doc/PROGRAMMER.READ.ME doc/READ.ME.FIRST.OR.DIE LICENSE.md

%files -n yaal-devel
%defattr(-,root,root,-)
%{_libdir}/lib*-d.so*
%{_libdir}/lib*.a
%{_includedir}/*
%{_datadir}/pkgconfig/*
%{_datadir}/yaal
%exclude %{_pkgdocdir}/html
%doc doc/INSTALL doc/PROGRAMMER.READ.ME doc/READ.ME.FIRST.OR.DIE LICENSE.md

%files -n yaal-doc
%defattr(-,root,root,-)
%doc build/doc/html doc/INSTALL doc/PROGRAMMER.READ.ME doc/READ.ME.FIRST.OR.DIE LICENSE.md

%changelog

