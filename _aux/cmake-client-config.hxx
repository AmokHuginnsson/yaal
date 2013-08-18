#ifndef @PROJECT_NAME_UC@_CONFIG_HXX_INCLUDED
#define @PROJECT_NAME_UC@_CONFIG_HXX_INCLUDED 1
#define PACKAGE_NAME "@PROJECT_NAME@"
#define PACKAGE_VERSION "0.0.0"
#define PACKAGE_STRING PACKAGE_NAME " " PACKAGE_VERSION
#define CXX_STANDARD @CXX_STANDARD@
#define LIB_INFIX "@LIB_INFIX@"
#cmakedefine HAVE_SGI_STL_EXTENSIONS
#ifndef __GNUC__
#define __attribute__( x ) /**/
#endif /* not __GNUC__ */
#ifdef __MSVCXX__
#define WIN32_LEAN_AND_MEAN 1
#define VC_EXTRALEAN 1
#pragma warning( disable : 4068 )
#pragma warning( disable : 4180 )
#pragma warning( disable : 4221 )
#pragma warning( disable : 4250 )
#pragma warning( disable : 4251 )
#pragma warning( disable : 4275 )
#pragma warning( disable : 4351 )
#pragma warning( disable : 4345 )
#pragma warning( disable : 4355 )
#pragma warning( disable : 4503 )
#pragma warning( disable : 4646 )
#pragma warning( disable : 4996 )
#undef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#define HAVE_PRETTY_FUNCTION 1
#include <yaal/cleanup.hxx>
#endif /* __MSVCXX__ */
#endif /* not @PROJECT_NAME_UC@_CONFIG_HXX_INCLUDED */

