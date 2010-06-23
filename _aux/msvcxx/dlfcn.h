#ifndef YAAL_MSVCXX_DLFCN_H_INCLUDED
#define YAAL_MSVCXX_DLFCN_H_INCLUDED 1

void* dlopen( char const*, int );
int dlclose( void* );
char const* dlerror( void );
void* dlsym( void*, char const* );

#define _DLFCN_H 1
#include <glibc/bits/dlfcn.h>

#endif /* not YAAL_MSVCXX_DLFCN_H_INCLUDED */

