#ifndef YAAL_MSVCXX_SYS_MMAN_H_INCLUDED
#define YAAL_MSVCXX_SYS_MMAN_H_INCLUDED 1

typedef char* caddr_t;

static int const PROT_NONE  = 0x00;
static int const PROT_EXEC  = 0x01;
static int const PROT_WRITE = 0x02;
static int const PROT_READ  = 0x04;

static int const MAP_PRIVATE = 0;

static void* const MAP_FAILED = reinterpret_cast<void*>( static_cast<size_t>( -1 ) );

void* mmap( void*, size_t, int, int, int, off_t );
int munmap( void*, size_t );

#endif /* not YAAL_MSVCXX_SYS_MMAN_H_INCLUDED */

