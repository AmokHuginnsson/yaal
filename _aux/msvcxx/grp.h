#ifndef YAAL_MSVCXX_GRP_H_INCLUDED
#define YAAL_MSVCXX_GRP_H_INCLUDED 1

typedef int gid_t;

struct group {
	char* gr_name;
};

int getgrgid_r( gid_t, struct group*, char*, int, struct group** );

#endif /* not YAAL_MSVCXX_GRP_H_INCLUDED */

