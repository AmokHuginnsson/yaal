#ifndef YAAL_MSVCXX_DLFCN_H_INCLUDED
#define YAAL_MSVCXX_DLFCN_H_INCLUDED 1

extern "C" void* dlopen( char const*, int );
extern "C" int dlclose( void* );
extern "C" char const* dlerror( void );
extern "C" void* dlsym( void*, char const* );

#define _DLFCN_H 1
#include <glibc/bits/dlfcn.h>
#define dlopen dlopen_fix

void* dlopen_fix( char const*, int );

#endif /* not YAAL_MSVCXX_DLFCN_H_INCLUDED */

