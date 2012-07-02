#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>

int main( int argc, char** argv ) {
	uid_t ruid = -1;
	uid_t euid = -1;
	uid_t suid = -1;
	int error = 0;
	do {

		if ( argc < 2 )
			break;

		if ( getresuid( &ruid, &euid, &suid ) != 0 ) {
			error = errno;
			break;
		}
		
		char const* username = getenv( "USERNAME" );
		if ( username == NULL ) {
			error = errno;
			break;
		}

		if ( euid == 0 ) {

			struct passwd pwd;
			static int const PWDBUF_SIZE = 1024;
			char pwdbuf[PWDBUF_SIZE];
			struct passwd* result = NULL;
			if ( getpwnam_r( username, &pwd, pwdbuf, PWDBUF_SIZE, &result ) ) {
				error = errno;
				break;
			}

			if ( getenv( "DROPPRIVS_DEBUG" ) != NULL ) {
				printf( "real user id: %d\n", ruid );
				printf( "effective user id: %d\n", euid );
				printf( "saved user id: %d\n", suid );
				printf( "true user id: %d\n", pwd.pw_uid );
			}

			if ( seteuid( pwd.pw_uid ) != 0 ) {
				error = errno;
				break;
			}

			if ( ruid == 0 ) {
				if ( setuid( pwd.pw_uid ) != 0 ) {
					error = errno;
					break;
				}
			}

		}

		execvp( argv[1], argv + 1 );
		error = errno;

	} while ( 0 );
	if ( error != 0 ) {
		printf( "%s\n", strerror( error ) );
	} else {
		printf( "invalid usage\n" );
	}
	return ( error );
}

