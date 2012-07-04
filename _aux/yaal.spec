Name:	yaal
Version:	0.0.30
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

%prep
cd -
umask 0077
make purge

%build
cd -
umask 0077
make %{?_smp_mflags} debug release doc PREFIX=%{_prefix} SYSCONFDIR=%{_sysconfdir} LIBDIR=%{_libdir}


%install
rm -rf ${RPM_BUILD_ROOT}
cd -
umask 0077
make install-debug install-release install-doc DESTDIR=${RPM_BUILD_ROOT}
rm -f ${RPM_BUILD_ROOT}/%_libdir/ld.so.conf ${RPM_BUILD_ROOT}/%{_prefix}/lib/ld.so.conf ${RPM_BUILD_ROOT}/%{_prefix}/lib/lib*[a-z]?.a ${RPM_BUILD_ROOT}/%{_prefix}/lib/mkcache

%clean
rm -rf ${RPM_BUILD_ROOT}
cd -
umask 0077
make purge

%files
%defattr(-,root,root,-)
%{_prefix}//lib/lib*[a-z]?.so*
%{_sysconfdir}/*

%files -n yaal-devel
%defattr(-,root,root,-)
%{_prefix}/lib/lib*-d.*
%{_includedir}/*
%{_datadir}/pkgconfig

%files -n yaal-doc
%defattr(-,root,root,-)
%{_datadir}/doc
%doc

%changelog

