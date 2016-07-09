#ifndef YAAL_MSVCXX_SYS_STATVFS_H_INCLUDED
#define YAAL_MSVCXX_SYS_STATVFS_H_INCLUDED 1

typedef int long long fsblkcnt_t;

struct statvfs {
	int long unsigned f_bsize;
	int long unsigned f_frsize;
	fsblkcnt_t f_blocks;
	fsblkcnt_t f_bfree;
	fsblkcnt_t f_bavail;
};

int statvfs( char const*, struct statvfs* );

#endif /* not YAAL_MSVCXX_SYS_STATVFS_H_INCLUDED */
// vim: ft=cpp
