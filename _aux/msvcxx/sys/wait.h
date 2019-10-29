#ifndef YAAL_MSVCXX_SYS_WAIT_H_INCLUDED
#define YAAL_MSVCXX_SYS_WAIT_H_INCLUDED 1

static int const WNOHANG   = 1;
static int const WUNTRACED = 2;

namespace msvcxx {
int waitpid( int, int*, int );
}

inline int waitpid( int pid_, int* status_, int options_ ) {
	return ( msvcxx::waitpid( pid_, status_, options_ ) );
}

constexpr int WEXITSTATUS( int status_ ) {
	return ( ( status_ & 0xff00 ) >> 8 );
}

constexpr int WTERMSIG( int status_ ) {
	return ( status_ & 0x7f );
}

constexpr bool WIFEXITED( int status_ ) {
	return ( WTERMSIG( status_ ) == 0  );
}

constexpr bool WIFSTOPPED( int status_ ) {
	return ( ( status_ & 0xff ) == 0x7f );
}

constexpr int WSTOPSIG( int status_ ) {
	return ( WEXITSTATUS( status_ ) );
}

constexpr bool WIFCONTINUED( int status_ ) {
	return ( status_ == 0xffff );
}

constexpr bool WIFSIGNALED( int status_ ) {
	return ( ! ( WIFSTOPPED( status_ ) || WIFEXITED( status_ ) ) );
}

#endif /* not YAAL_MSVCXX_SYS_WAIT_H_INCLUDED */

