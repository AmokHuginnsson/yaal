# What is the best current function name preprocessor capability?

set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11" )
if ( NOT CMAKE_HOST_WIN32 )
	set( CMAKE_REQUIRED_FLAGS_ORIG "${CMAKE_REQUIRED_FLAGS}" )
	set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Wall -Werror" )
endif ( NOT CMAKE_HOST_WIN32 )
check_cxx_source_compiles( "int main( int, char** ) { char const* p( __PRETTY_FUNCTION__ ); return ( 0 ); }" HAVE_PRETTY_FUNCTION )
if ( NOT HAVE_PRETTY_FUNCTION )
	check_cxx_source_compiles( "int main( int, char** ) { char const* p( __FUNCTION__ ); return ( 0 ); }" HAVE_FUNCTION )
	if ( NOT HAVE_FUNCTION )
		check_cxx_source_compiles( "int main( int, char** ) { char const* p( __func__ ); return ( 0 ); }" HAVE_FUNC )
	endif()
endif()
if ( NOT CMAKE_HOST_WIN32 )
	set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_ORIG}" )
endif ( NOT CMAKE_HOST_WIN32 )

# Does strftime support getting required buffer lenght?

check_cxx_source_compiles( "#include <ctime>
int main( int, char const* const* ) {
	time_t t( time( nullptr ) );
	struct tm broken;
	localtime_r( &t, &broken );
	int size( strftime( nullptr, 1024, \"%Y-%m-%d %T\", &broken ) );
	return ( ! size );
}" HAVE_SMART_STRFTIME )

# Is iconv()'s source buffer a pointer to const type?

check_cxx_source_compiles( "#include <iconv.h>
int main( int, char const* const* ) {
	iconv_t conv( 0 );
	size_t size( 0 );
	char const* in( nullptr );
	char* out( nullptr );
	iconv( conv, &in, &size, &out, &size );
	return ( 0 );
}" HAVE_ICONV_INPUT_CONST )

# Is gethostbyname_r() of GNU flavor?

check_cxx_source_compiles( "#include <netdb.h>
int main( int, char const* const* ) {
	int i( 0 );
	char* p( 0 );
	struct hostent h, * ph( 0 );
	::gethostbyname_r( \"\", &h, p, 0, &ph, &i );
	return ( 0 );
}" HAVE_GNU_GETHOSTBYNAME_R )

# Is gethostbyaddr_r() of GNU flavor?

check_cxx_source_compiles( "#include <netdb.h>
int main( int, char const* const* ) {
	int i( 0 );
	char* p( 0 );
	struct hostent h, * ph( 0 );
	::gethostbyaddr_r( 0, 0, 0, &h, p, 0, &ph, &i );
	return ( 0 );
}" HAVE_GNU_GETHOSTBYADDR_R )

# Is basename() declaration located in cstring header?

check_cxx_source_compiles( "#include <cstring>
int main( int, char** ) {
	char* p( nullptr );
	basename( p );
	return ( 0 );
}" HAVE_BASENAME_IN_CSTRING )

# Is basename()'s argumnt type a pointer to const type?

check_cxx_source_compiles( "#include <cstring>
int main( int, char const* const* ) {
	char const* p( nullptr );
	basename( p );
	return ( 0 );
}" HAVE_BASENAME_ARG_CONST )

# Does ncurses have ascii graphics?

if ( HAVE_NCURSES_CURSES_H )
	set( CURSES_PATH "ncurses/curses.h" )
else ( HAVE_NCURSES_CURSES_H )
	set( CURSES_PATH "curses.h" )
endif ( HAVE_NCURSES_CURSES_H )
set( CMAKE_REQUIRED_LIBRARIES ${CURSES_LIBRARIES} )
check_cxx_source_compiles( "#include <${CURSES_PATH}>
	int main( int, char** ) { addch(ACS_DARROW); return ( 0 ); }" HAVE_ASCII_GRAPHICS )
set( CMAKE_REQUIRED_LIBRARIES )

# What is the second arg type in attr_get()?

if ( CMAKE_HOST_WIN32 )
	set( NCURSES_ATTR_GET_SECOND_ARG_TYPE_int_short 1 )
else ( CMAKE_HOST_WIN32 )
	set( CMAKE_REQUIRED_LIBRARIES ${CURSES_LIBRARIES} )
	set( CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Wall -Werror -Wcast-align -Wconversion -Wwrite-strings -pedantic-errors -Wcast-qual" )
	check_cxx_source_compiles( "#include <${CURSES_PATH}>
		int main( int, char** ){ attr_t* a( nullptr ); int short* p( nullptr ); attr_get( a, p, nullptr ); return( 0 ); }" NCURSES_ATTR_GET_SECOND_ARG_TYPE_int_short )
	check_cxx_source_compiles( "#include <${CURSES_PATH}>
		int main( int, char** ){ attr_t* a( nullptr ); int* p( nullptr ); attr_get( a, p, nullptr ); return( 0 ); }" NCURSES_ATTR_GET_SECOND_ARG_TYPE_int )
	set( CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_ORIG} )
	set( CMAKE_REQUIRED_LIBRARIES )
endif ( CMAKE_HOST_WIN32 )
if ( NCURSES_ATTR_GET_SECOND_ARG_TYPE_int_short )
	set( NCURSES_ATTR_GET_SECOND_ARG_TYPE "int short" )
elseif ( NCURSES_ATTR_GET_SECOND_ARG_TYPE_int  )
	set( NCURSES_ATTR_GET_SECOND_ARG_TYPE "int" )
else ( NCURSES_ATTR_GET_SECOND_ARG_TYPE_int )
	message( FATAL_ERROR "Unknown type for second argument of attr_get in ncurses library!" )
endif ( NCURSES_ATTR_GET_SECOND_ARG_TYPE_int_short )

