cmake_minimum_required( VERSION 2.6 )
if ( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE debug CACHE STRING "Choose the type of build, options are: debug, release." FORCE )
endif ( NOT CMAKE_BUILD_TYPE )
if ( ( NOT ( "${CMAKE_BUILD_TYPE}" STREQUAL "debug" ) ) AND ( NOT ( "${CMAKE_BUILD_TYPE}" STREQUAL "release" ) ) )
	message( FATAL_ERROR "Only `debug' and `release' tergets are supported." )
endif ( ( NOT ( "${CMAKE_BUILD_TYPE}" STREQUAL "debug" ) ) AND ( NOT ( "${CMAKE_BUILD_TYPE}" STREQUAL "release" ) ) )

if ( "${CMAKE_BUILD_TYPE}" STREQUAL "debug" )
	message( "Generating debug build." )
	set( LIB_INFIX "-d" )
else ( "${CMAKE_BUILD_TYPE}" STREQUAL "release" )
	message( "Generating release build." )
	set( LIB_INFIX "" )
endif ( "${CMAKE_BUILD_TYPE}" STREQUAL "debug" )

if ( NOT ( "$ENV{PREFIX}" STREQUAL "" ) )
	set( CMAKE_INSTALL_PREFIX $ENV{PREFIX} CACHE STRING "Install prefix." )
endif ( NOT ( "$ENV{PREFIX}" STREQUAL "" ) )

#set( CMAKE_VERBOSE_MAKEFILE true )
enable_language( C )
project( yaal CXX )
set( COMPONENTS hcore tools dbwrapper hconsole hdata )
get_filename_component( yaal_SOURCE_DIR ${yaal_SOURCE_DIR}/../ ABSOLUTE )
message( "yaal_SOURCE_DIR: " ${yaal_SOURCE_DIR} )
add_definitions( -D__YAAL_BUILD__ -D__ID__="" -D__TID__="" )
if ( CMAKE_HOST_WIN32 )
	add_definitions( -D__MSVCXX__ -D_GNU_SOURCE )
	set_property( DIRECTORY APPEND PROPERTY INCLUDE_DIRECTORIES "${yaal_SOURCE_DIR}/_aux/msvcxx;$(VCInstallDir)/include;$ENV{CMAKE_INCLUDE_PATH}; $ENV{CMAKE_INCLUDE_PATH}/glibc; $ENV{CMAKE_INCLUDE_PATH}/winx;${yaal_SOURCE_DIR}" )
	link_directories( $ENV{CMAKE_LIBRARY_PATH} )
	set( LIB_PREFIX "" )
	set( LIB_EXT "dll" )
	set( SYMBOL_PREFIX "" )
else ( CMAKE_HOST_WIN32 )
	include_directories( ${yaal_SOURCE_DIR} )
	set( LIB_PREFIX "lib" )
	set( LIB_EXT "so" )
	set( SYMBOL_PREFIX "" )
endif ( CMAKE_HOST_WIN32 )
include( CheckFunctionExists )
if ( CMAKE_HOST_WIN32 )
	set( CMAKE_REQUIRED_INCLUDES "${yaal_SOURCE_DIR}/_aux/msvcxx;\\\\.\\$(VCInstallDir)/include;$ENV{CMAKE_INCLUDE_PATH}; $ENV{CMAKE_INCLUDE_PATH}/glibc; $ENV{CMAKE_INCLUDE_PATH}/winx;${yaal_SOURCE_DIR}" )
	set( CMAKE_REQUIRED_LIBRARIES "$ENV{CMAKE_LIBRARY_PATH}/libgw32c.a" )
	set( CMAKE_REQUIRED_DEFINITIONS -D__value=value_ )
endif ( CMAKE_HOST_WIN32 )
set( CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} -D_GNU_SOURCE )
check_function_exists( strcasestr HAVE_STRCASESTR )
check_function_exists( asprintf HAVE_ASPRINTF )
check_function_exists( strtold HAVE_STRTOLD )
check_function_exists( gethostbyname_r HAVE_GETHOSTBYNAME_R )
check_function_exists( gethostbyaddr_r HAVE_GETHOSTBYADDR_R )
check_function_exists( memrchr HAVE_MEMRCHR )
include( CheckSymbolExists )
check_symbol_exists( TEMP_FAILURE_RETRY unistd.h HAVE_DECL_TEMP_FAILURE_RETRY )
check_symbol_exists( getaddrinfo netdb.h;netinet/in.h;sys/socket.h HAVE_DECL_GETADDRINFO )
check_symbol_exists( SUN_LEN sys/un.h HAVE_DECL_SUN_LEN )
check_symbol_exists( B76800 termios.h HAVE_DECL_B76800 )
check_symbol_exists( B28800 termios.h HAVE_DECL_B28800 )
check_symbol_exists( B14400 termios.h HAVE_DECL_B14400 )
check_symbol_exists( B7200 termios.h HAVE_DECL_B7200 )
check_symbol_exists( SIGIOT csignal HAVE_DECL_SIGIOT )
check_symbol_exists( VSWTC termios.h HAVE_DECL_VSWTC )
check_symbol_exists( ERR cstdlib HAVE_DECL_ERR )
if ( CMAKE_HOST_WIN32 )
	set( CMAKE_REQUIRED_INCLUDES )
	set( CMAKE_REQUIRED_LIBRARIES )
endif ( CMAKE_HOST_WIN32 )
set( CMAKE_REQUIRED_DEFINITIONS )
include( CheckIncludeFileCXX )
check_include_file_cxx( sqlite3.h HAVE_SQLITE3_H )
if ( HAVE_SQLITE3_H )
	list( APPEND DRIVERS sqlite3 )
endif ( HAVE_SQLITE3_H )
check_include_file_cxx( postgresql/libpq-fe.h HAVE_POSTGRESQL_LIBPQ_FE_H )
check_include_file_cxx( libpq-fe.h HAVE_LIBPQ_FE_H )
if ( HAVE_LIBPQ_FE_H OR HAVE_POSTGRESQL_LIBPQ_FE_H )
	list( APPEND DRIVERS postgresql )
endif ( HAVE_LIBPQ_FE_H OR HAVE_POSTGRESQL_LIBPQ_FE_H )
check_include_file_cxx( mysql/mysql.h HAVE_MYSQL_MYSQL_H )
if ( HAVE_MYSQL_MYSQL_H )
	list( APPEND DRIVERS mysql )
endif ( HAVE_MYSQL_MYSQL_H )
check_include_file_cxx( oci.h HAVE_OCI_H )
if ( HAVE_OCI_H )
	list( APPEND DRIVERS oracle )
endif ( HAVE_OCI_H )
find_package( Threads REQUIRED )
find_package( LibXml2 REQUIRED )
if ( CMAKE_HOST_WIN32 )
	set_property( DIRECTORY APPEND PROPERTY INCLUDE_DIRECTORIES "$ENV{CMAKE_INCLUDE_PATH}/libxml2" )
else ( CMAKE_HOST_WIN32 )
	set_property( DIRECTORY APPEND PROPERTY INCLUDE_DIRECTORIES /usr/include/libxml2 )
endif ( CMAKE_HOST_WIN32 )
find_package( LibXslt REQUIRED )
find_package( OpenSSL REQUIRED )
find_package( ZLIB REQUIRED )
find_package( Curses REQUIRED )
check_include_file_cxx( execinfo.h HAVE_EXECINFO_H )
check_include_file_cxx( sys/consio.h HAVE_SYS_CONSIO_H )
check_include_file_cxx( gpm.h HAVE_GPM_H )
check_include_file_cxx( ncurses/curses.h HAVE_NCURSES_CURSES_H )
check_include_file_cxx( curses.h HAVE_CURSES_H )
if ( HAVE_NCURSES_CURSES_H )
	set( CURSES_PATH "ncurses/curses.h" )
else ( HAVE_NCURSES_CURSES_H )
	set( CURSES_PATH "curses.h" )
endif ( HAVE_NCURSES_CURSES_H )
include( CheckCXXSourceCompiles )
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::powl( 0., 0. ); return ( 0 ); }" HAVE_POWL )
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::floorl( 0. ); return ( 0 ); }" HAVE_DECL_FLOORL )
check_cxx_source_compiles("#include <netdb.h>
int main( int, char const* const* const )
	{
	int i( 0 );
	char* p( 0 );
	struct hostent h, * ph( 0 );
	::gethostbyname_r( \"\", &h, p, 0, &ph, &i );
	return ( 0 );
	}" HAVE_GNU_GETHOSTBYNAME_R )
check_cxx_source_compiles("#include <netdb.h>
int main( int, char const* const* const )
	{
	int i( 0 );
	char* p( 0 );
	struct hostent h, * ph( 0 );
	::gethostbyaddr_r( 0, 0, 0, &h, p, 0, &ph, &i );
	return ( 0 );
	}" HAVE_GNU_GETHOSTBYADDR_R )
check_cxx_source_compiles("#include <cstring>
int main( int, char** )
	{
	char* p( NULL );
	basename( p );
	return ( 0 );
	}" HAVE_BASENAME_IN_CSTRING )
check_cxx_source_compiles("#include<cstring>
int main( int, char const* const* const )
	{
	char const* const p( NULL );
	basename( p );
	return ( 0 );
	}" HAVE_BASENAME_ARG_CONST )
check_cxx_source_compiles("#include <ctime>
int main( int, char const* const* const )
	{
	time_t t( time( NULL ) );
	struct tm broken;
	localtime_r( &t, &broken );
	int size( strftime( NULL, 1024, \"%F %T\", &broken ) );
	return ( ! size );
	}" HAVE_SMART_STRFTIME )
check_cxx_source_compiles("#include <iconv.h>
int main( int, char const* const* const )
	{
	iconv_t conv( 0 );
	size_t size( 0 );
	char const* in( NULL );
	char* out( NULL );
	iconv( conv, &in, &size, &out, &size );
	return ( 0 );
	}" HAVE_ICONV_INPUT_CONST )
if ( CMAKE_HOST_WIN32 )
	add_definitions( -D__value=value_ /FIfix.hxx )
	set( NCURSES_ATTR_GET_SECOND_ARG_TYPE_int 1 )
else ( CMAKE_HOST_WIN32 )
	set( CMAKE_REQUIRED_LIBRARIES ${CURSES_LIBRARIES} )
	set( CMAKE_REQUIRED_FLAGS "-Wall -Werror -Wcast-align -Wconversion -Wwrite-strings -pedantic-errors -Wcast-qual" )
	check_cxx_source_compiles("#include <${CURSES_PATH}>
		int main( int, char** ){ attr_t* a( NULL ); int short* p( NULL ); attr_get( a, p, NULL ); return( 0 ); }" NCURSES_ATTR_GET_SECOND_ARG_TYPE_int_short)
	check_cxx_source_compiles("#include <${CURSES_PATH}>
		int main( int, char** ){ attr_t* a( NULL ); int* p( NULL ); attr_get( a, p, NULL ); return( 0 ); }" NCURSES_ATTR_GET_SECOND_ARG_TYPE_int)
	set( CMAKE_REQUIRED_FLAGS )
	set( CMAKE_REQUIRED_LIBRARIES )
endif ( CMAKE_HOST_WIN32 )
if ( NCURSES_ATTR_GET_SECOND_ARG_TYPE_int_short )
	set( NCURSES_ATTR_GET_SECOND_ARG_TYPE "int short" )
elseif ( NCURSES_ATTR_GET_SECOND_ARG_TYPE_int  )
	set( NCURSES_ATTR_GET_SECOND_ARG_TYPE "int" )
else ( NCURSES_ATTR_GET_SECOND_ARG_TYPE_int )
	message( FATAL_ERROR "Unknown type for second argument of attr_get in ncurses library!")
endif ( NCURSES_ATTR_GET_SECOND_ARG_TYPE_int_short )
configure_file( ${yaal_SOURCE_DIR}/_aux/config.hxx.cmake config.hxx )

macro( yaal_make_component name )
	message( "Processing component: " ${name} )
	file( GLOB SRCS RELATIVE ${yaal_SOURCE_DIR} ${yaal_SOURCE_DIR}/${name}/*.cxx )
	file( GLOB HDRS RELATIVE ${yaal_SOURCE_DIR} ${yaal_SOURCE_DIR}/${name}/*.hxx )
	file( GLOB DRIVER RELATIVE ${yaal_SOURCE_DIR} ${yaal_SOURCE_DIR}/${name}/*_driver.cxx )
	list( REMOVE_ITEM SRCS ${DRIVER} dummy_item )
	add_library( ${name} SHARED ${SRCS} ${HDRS} )
	set( HEADERS ${HEADERS} ${HDRS} )
	if ( CMAKE_HOST_WIN32 )
		add_library( ${name}-static STATIC ${SRCS} ${HDRS} )
		set_target_properties( ${name}-static PROPERTIES OUTPUT_NAME ${name} )
		set_target_properties( ${name}-static PROPERTIES PREFIX "lib" )
		add_dependencies( ${name} ${name}-static )
		get_target_property( TARGET_NAME ${name}-static LOCATION )
		get_filename_component( TARGET_PATH ${TARGET_NAME} PATH )
		add_custom_command( TARGET ${name}-static POST_BUILD COMMAND dumpbin /linkermember:1 ${TARGET_NAME} /out:${TARGET_PATH}/${name}${LIB_INFIX}.sym )
		add_custom_command( TARGET ${name}-static POST_BUILD COMMAND makedef ${TARGET_PATH}/${name}${LIB_INFIX}.sym ${TARGET_PATH}/${name}${LIB_INFIX}.def _yaal_hcore_main,_yaal_tools_main,_yaal_dbwrapper_main,_yaal_hconsole_main,_yaal_hdata_main,__CTA1H,__TI1H,_acosl,_asinl,_atanl,_coshl,_cosl,_expl,_floorl,_log10l,_logl,_powl,_sinhl,_sinl,_sqrtl,_tanhl,_tanl )
		set_target_properties( ${name} PROPERTIES LINK_FLAGS "/DEF:${TARGET_PATH}/${name}${LIB_INFIX}.def /IGNORE:4102" )
	endif ( CMAKE_HOST_WIN32 )
	set_target_properties( ${name} PROPERTIES OUTPUT_NAME ${name}${LIB_INFIX} LINKER_LANGUAGE CXX VERSION 0.0 SOVERSION 0.0 LIBRARY_OUTPUT_DIRECTORY build/${CMAKE_BUILD_TYPE} )
endmacro( yaal_make_component )

macro( yaal_make_driver name )
	message( "Processing driver: " ${name} )
	add_library( ${name} MODULE dbwrapper/${name}_driver.cxx )
	set_target_properties( ${name} PROPERTIES OUTPUT_NAME ${name}_driver${LIB_INFIX} LINKER_LANGUAGE CXX VERSION 0.0 SOVERSION 0.0 LIBRARY_OUTPUT_DIRECTORY build/${CMAKE_BUILD_TYPE} )
endmacro( yaal_make_driver )

foreach( COMPONENT ${COMPONENTS} )
	yaal_make_component( "${COMPONENT}" )
endforeach( COMPONENT )

foreach( DRIVER ${DRIVERS} )
	yaal_make_driver( "${DRIVER}" )
endforeach( DRIVER )

add_dependencies( tools hcore )
add_dependencies( dbwrapper tools hcore )
add_dependencies( hconsole tools hcore )
add_dependencies( hdata hconsole dbwrapper tools hcore )

get_target_property( TARGET_NAME hcore LOCATION )
get_filename_component( TARGET_PATH ${TARGET_NAME} PATH )

if ( CMAKE_HOST_WIN32 )
	add_dependencies( hcore msvcxx )
	target_link_libraries( hcore libgw32c libintl pthreadVCE2 libeay32MDd ssleay32MDd Ws2_32 Dbghelp regex curses msvcxx libgcc )
	target_link_libraries( tools hcore libxml2 libxslt zlib libiconv )
	target_link_libraries( dbwrapper tools )
	target_link_libraries( hconsole tools hcore )
	target_link_libraries( hdata hconsole dbwrapper tools hcore )
	message( "Processing component: " msvcxx )
	file( GLOB SRCS ${yaal_SOURCE_DIR}/_aux/msvcxx/*.cxx )
	file( GLOB HDRS ${yaal_SOURCE_DIR}/_aux/msvcxx/*.hxx ${yaal_SOURCE_DIR}/_aux/msvcxx/*.h ${yaal_SOURCE_DIR}/_aux/msvcxx/*/*.h )
	add_library( msvcxx STATIC ${SRCS} ${HDRS} ${yaal_SOURCE_DIR}/config.hxx )
	set_target_properties( msvcxx PROPERTIES LINKER_LANGUAGE CXX LIBRARY_OUTPUT_DIRECTORY build/${CMAKE_BUILD_TYPE} )
	add_executable( makedef ${yaal_SOURCE_DIR}/_aux/makedef.cxx )
	add_executable( mkheaders ${yaal_SOURCE_DIR}/_aux/mkheaders.cxx )
	add_dependencies( msvcxx makedef mkheaders )
	set_target_properties( makedef PROPERTIES COMPILE_DEFINITIONS "YAAL_MSVCXX_FIX_HXX_INCLUDED=1" )
	set_target_properties( mkheaders PROPERTIES COMPILE_DEFINITIONS "YAAL_MSVCXX_FIX_HXX_INCLUDED=1" )
	add_custom_target( headers ALL COMMAND mkheaders ${yaal_SOURCE_DIR} ${TARGET_PATH} "${HEADERS}" DEPENDS hdata )
else ( CMAKE_HOST_WIN32 )
	target_link_libraries( hcore ssl pthread rt nsl )
	target_link_libraries( tools hcore xml2 xslt z )
	target_link_libraries( dbwrapper tools hcore )
	target_link_libraries( hconsole tools hcore ncurses gpm )
	target_link_libraries( hdata hconsole dbwrapper tools hcore )
	add_custom_target( headers ALL COMMAND env DIR_ROOT=${yaal_SOURCE_DIR} DIR_BUILD=${TARGET_PATH} HDRS='${HEADERS}' ./_aux/mkheaders DEPENDS hdata )
	set_target_properties( headers PROPERTIES PUBLIC_HEADER ${HEADERS} )
endif ( CMAKE_HOST_WIN32 )

install( TARGETS ${COMPONENTS} ${DRIVERS} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib )
install( DIRECTORY ${TARGET_PATH}/yaal DESTINATION include )

