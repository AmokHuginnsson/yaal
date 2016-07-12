#ifndef YAAL_MSVCXX_DIRENT_H_INCLUDED
#define YAAL_MSVCXX_DIRENT_H_INCLUDED 1

#include "hcore/macro.hxx"

static int const NAME_MAX = 255;
static int const DT_UNKNOWN = 0;
static int const DT_DIR = 4;
static int const DT_BLK = 6;
static int const DT_REG = 8;

struct dirent {
	int d_ino;
	int d_type;
	char d_name[NAME_MAX + 1];
};

struct DIR;

DIR* opendir( char const* );
int closedir( DIR* );
int readdir_r( struct DIR*, struct dirent*, struct dirent** );
int telldir( DIR* );
void seekdir( DIR*, int );

#endif /* not YAAL_MSVCXX_DIRENT_H_INCLUDED */

