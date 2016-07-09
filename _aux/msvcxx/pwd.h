#ifndef YAAL_MSVCXX_PWD_H_INCLUDED
#define YAAL_MSVCXX_PWD_H_INCLUDED 1

struct passwd {
	char* pw_name;
};

int getpwuid_r( uid_t, struct passwd*, char*, int, struct passwd** );

#endif /* not YAAL_MSVCXX_PWD_H_INCLUDED */

