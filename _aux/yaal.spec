Name:	yaal
Version:	%(awk '/^VERSION *=|SUBVERSION *=|EXTRAVERSION *=/{VERSION=VERSION DOT $3;DOT="."}END{print VERSION}' ./Makefile.mk.in)
Release:	1%{?dist}
Summary:	Yet Another Abstraction Layer - general purpose C++ library.

Group:		System Environment/Libraries
License:	Commercial
URL:			http://codestation.org/
Source:		.
# Source: https://codestation.org/repo/yaal.git
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:	make, autoconf, libtool, libxml2-devel libxslt-devel, zlib-devel, openssl-devel, ncurses-devel, doxygen
Requires:	libxml2, libxslt, openssl, zlib, ncurses

%description

%package -n yaal-devel
Summary:	Yet Another Abstraction Layer - general purpose C++ library - development files.
%description -n yaal-devel

%package -n yaal-doc
Summary:	Yet Another Abstraction Layer - general purpose C++ library - developer documentation.
%description -n yaal-doc

%define flagfilter sed -e 's/-O2\\>//g' -e 's/-g\\>//g' -e 's/-Wall\\>//g' -e 's/-Werror\\>//g' -e 's/-Wformat-security\\>//g' -e 's/-Wformat\\>//g'
%define clearflags export CFLAGS=`echo ${CFLAGS} | %{flagfilter}`;export CXXFLAGS=`echo ${CXXFLAGS} | %{flagfilter}`

%prep
umask 0077
if [ -f "${RPM_SOURCE_DIR}/%{name}-%{version}.tar.gz" ] ; then
%setup
else
	cd -
	/bin/rm -rf "${RPM_BUILD_DIR}/%{name}-%{version}"
	mkdir "${RPM_BUILD_DIR}/%{name}-%{version}"
	tar cf - --exclude build . | tar -x -C "${RPM_BUILD_DIR}/%{name}-%{version}"
	cd -
fi
cd "${RPM_BUILD_DIR}/%{name}-%{version}"
make purge

%build
umask 0077
%{clearflags}
make %{?_smp_mflags} debug release doc PREFIX=%{_prefix} SYSCONFDIR=%{_sysconfdir} LIBDIR=%{_libdir}

%install
rm -rf ${RPM_BUILD_ROOT}
umask 0077
make install-debug install-release install-doc DESTDIR=${RPM_BUILD_ROOT}
rm -f ${RPM_BUILD_ROOT}/%{_libdir}/ld.so.conf ${RPM_BUILD_ROOT}/%{_libdir}/ld.so.cache ${RPM_BUILD_ROOT}/%{_libdir}/mkcache

%clean
rm -rf ${RPM_BUILD_ROOT}
umask 0077
make purge

%files
%defattr(-,root,root,-)
%{_libdir}/lib*[a-z]?.so*
%{_sysconfdir}/*

%files -n yaal-devel
%defattr(-,root,root,-)
%{_libdir}/lib*-d.so*
%{_libdir}/lib*.a
%{_includedir}/*
%{_datadir}/pkgconfig
%{_datadir}/yaal

%files -n yaal-doc
%defattr(-,root,root,-)
%{_datadir}/doc
%doc

%changelog

