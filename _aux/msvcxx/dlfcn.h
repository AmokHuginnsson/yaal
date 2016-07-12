#ifndef YAAL_MSVCXX_DLFCN_H_INCLUDED
#define YAAL_MSVCXX_DLFCN_H_INCLUDED 1

static int const RTLD_NOW    = 0x0002;
static int const RTLD_GLOBAL = 0x0100;

extern "C" void* dlopen( char const*, int );
extern "C" int dlclose( void* );
extern "C" char const* dlerror( void );
extern "C" void* dlsym( void*, char const* );

#endif /* not YAAL_MSVCXX_DLFCN_H_INCLUDED */

