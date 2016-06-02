#ifndef YAAL_MSVCXX_DLFCN_H_INCLUDED
#define YAAL_MSVCXX_DLFCN_H_INCLUDED 1

extern "C" void* dlopen( char const*, int );
extern "C" int dlclose( void* );
extern "C" char const* dlerror( void );
extern "C" void* dlsym( void*, char const* );

#endif /* not YAAL_MSVCXX_DLFCN_H_INCLUDED */

