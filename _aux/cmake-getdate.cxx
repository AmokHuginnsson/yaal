#include <cstdio>
#include <ctime>
int main( int argc, char** argv ) {
	time_t t( time( 0 ) );
	tm* ti( localtime( &t ) );
	printf( "%d%02d%02d\n", ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday );
	return ( 0 );
}
