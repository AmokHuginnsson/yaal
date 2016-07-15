#ifndef YAAL_MSVCXX_DLFCN_H_INCLUDED
#define YAAL_MSVCXX_DLFCN_H_INCLUDED 1

static int const RTLD_NOW    = 0x0002;
static int const RTLD_GLOBAL = 0x0100;

void* dlopen( char const*, int );
int dlclose( void* );
char const* dlerror( void );
void* dlsym( void*, char const* );

#endif /* not YAAL_MSVCXX_DLFCN_H_INCLUDED */

