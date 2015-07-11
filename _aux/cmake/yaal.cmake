cmake_minimum_required( VERSION 2.8.4 )

project( yaal CXX )
include( common )

set( PACKAGE_VERSION "\"${PROJECT_VERSION}.${PROJECT_SUBVERSION}.${PROJECT_EXTRAVERSION}\"" )

include( types )

if ( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
	set( TARGET_CPU_BITS 64 )
else()
	set( TARGET_CPU_BITS 32 )
endif()

add_definitions( -D__YAAL_BUILD__ )
if ( CMAKE_HOST_WIN32 )
	if ( MSVC11 )
		set( CMAKE_SUPPRESS_REGENERATION TRUE )
	endif ( MSVC11 )
	add_definitions( -D__MSVCXX__ -D__value=value_ /FIfix.hxx -D_USE_32BIT_TIME_T )
	include_directories( BEFORE "\\\\.\\$(VCInstallDir)/include" "${CMAKE_HOME_DIRECTORY}/_aux/msvcxx" )
	include_directories( "${CMAKE_INCLUDE_PATH}/glibc" "${CMAKE_INCLUDE_PATH}/libxml2" )
	set( LIB_PREFIX "" )
	set( LIB_EXT "dll" )
	set( SYMBOL_PREFIX "" )
	set( EXE_SUFFIX ".exe" )
	set( CMAKE_REQUIRED_INCLUDES "${CMAKE_HOME_DIRECTORY}/_aux/msvcxx;\\\\.\\$(VCInstallDir)/include;${CMAKE_INCLUDE_PATH}; ${CMAKE_INCLUDE_PATH}/glibc; ${CMAKE_HOME_DIRECTORY}" )
	set( CMAKE_REQUIRED_LIBRARIES ws2_32.lib "${CMAKE_LIBRARY_PATH}/libgw32c.lib" )
	set( CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} -D__value=value_ )
else ( CMAKE_HOST_WIN32 )
	set( LIB_PREFIX "lib" )
	set( LIB_EXT "so" )
	set( SYMBOL_PREFIX "" )
	set( EXE_SUFFIX "" )
	include_directories( /usr/include/libxml2 )
endif ( CMAKE_HOST_WIN32 )

set( COMPONENTS hcore tools dbwrapper hconsole hdata )

include( packages )

cxx_search_libraries( HCORE_LIBS timer_create rt )

check_include_file_cxx( pcreposix.h HAVE_PCREPOSIX_H )
check_include_file_cxx( execinfo.h HAVE_EXECINFO_H )
check_include_file_cxx( termio.h HAVE_TERMIO_H )
check_include_file_cxx( tty.h HAVE_TTY_H )
check_include_file_cxx( sys/ioctl.h HAVE_SYS_IOCTL_H )
check_include_file_cxx( sys/consio.h HAVE_SYS_CONSIO_H )
check_include_file_cxx( gpm.h HAVE_GPM_H )
check_include_file_cxx( ncurses/curses.h HAVE_NCURSES_CURSES_H )
check_include_file_cxx( curses.h HAVE_CURSES_H )
set( CMAKE_EXTRA_INCLUDE_FILES signal.h )
check_type_size( sighandler_t HAVE_SIGHANDLER_T )
if ( NOT HAVE_SIGHANDLER_T )
	check_type_size( __sighandler_t HAVE___SIGHANDLER_T )
	if ( NOT HAVE___SIGHANDLER_T )
		check_type_size( SIG_PF HAVE_SIG_PF )
		if ( NOT HAVE_SIG_PF )
			if ( CMAKE_HOST_WIN32 )
				set( HAVE_SIGHANDLER_T 1 )
			else ( CMAKE_HOST_WIN32 )
				message( FATAL_ERROR "Cannot continue without signal handler type definition." )
			endif ( CMAKE_HOST_WIN32 )
		endif ( NOT HAVE_SIG_PF )
	endif( NOT HAVE___SIGHANDLER_T )
endif ( NOT HAVE_SIGHANDLER_T )
set( CMAKE_EXTRA_INCLUDE_FILES )
check_function_exists( strcasestr HAVE_STRCASESTR )
check_function_exists( strtold HAVE_DECL_STRTOLD )
check_function_exists( pthread_setname_np HAVE_PTHREAD_SETNAME_NP )
check_function_exists( pthread_set_name_np HAVE_PTHREAD_SET_NAME_NP )
check_function_exists( prctl HAVE_PRCTL )
check_function_exists( gethostbyname_r HAVE_GETHOSTBYNAME_R )
check_function_exists( gethostbyaddr_r HAVE_GETHOSTBYADDR_R )
if ( CMAKE_HOST_WIN32 )
	check_symbol_exists( getaddrinfo Ws2tcpip.h HAVE_GETADDRINFO )
	check_symbol_exists( getnameinfo Ws2tcpip.h HAVE_GETNAMEINFO )
else()
	check_function_exists( getaddrinfo HAVE_GETADDRINFO )
	check_function_exists( getnameinfo HAVE_GETNAMEINFO )
endif()
check_function_exists( clock_gettime HAVE_CLOCK_GETTIME )
check_function_exists( timer_create HAVE_TIMER_CREATE )
check_function_exists( memrchr HAVE_MEMRCHR )
check_function_exists( getline HAVE_GETLINE )
check_symbol_exists( TEMP_FAILURE_RETRY unistd.h HAVE_DECL_TEMP_FAILURE_RETRY )
check_symbol_exists( SUN_LEN sys/un.h HAVE_DECL_SUN_LEN )
check_symbol_exists( RLIMIT_AS sys/resource.h HAVE_DECL_RLIMIT_AS )
check_symbol_exists( RLIMIT_NPROC sys/resource.h HAVE_DECL_RLIMIT_NPROC )
check_symbol_exists( B76800 termios.h HAVE_DECL_B76800 )
check_symbol_exists( B28800 termios.h HAVE_DECL_B28800 )
check_symbol_exists( B14400 termios.h HAVE_DECL_B14400 )
check_symbol_exists( B7200 termios.h HAVE_DECL_B7200 )
check_symbol_exists( SIGIOT csignal HAVE_DECL_SIGIOT )
check_symbol_exists( VSWTC termios.h HAVE_DECL_VSWTC )
check_symbol_exists( ERR cstdlib HAVE_DECL_ERR )
check_include_file_cxx( sqlite3.h HAVE_SQLITE3_H )
set( DEFAULT_DRIVER "null" )
if ( HAVE_SQLITE3_H )
	list( APPEND DRIVERS sqlite3 )
	set( DEFAULT_DRIVER "SQLite3" )
endif ( HAVE_SQLITE3_H )
check_include_file_cxx( postgresql/libpq-fe.h HAVE_POSTGRESQL_LIBPQ_FE_H )
check_include_file_cxx( libpq-fe.h HAVE_LIBPQ_FE_H )
if ( HAVE_LIBPQ_FE_H OR HAVE_POSTGRESQL_LIBPQ_FE_H )
	list( APPEND DRIVERS postgresql )
	if ( "${DEFAULT_DRIVER}" STREQUAL "" )
		set( DEFAULT_DRIVER "PostgreSQL" )
	endif()
endif ( HAVE_LIBPQ_FE_H OR HAVE_POSTGRESQL_LIBPQ_FE_H )
check_include_file_cxx( mysql/mysql.h HAVE_MYSQL_MYSQL_H )
if ( HAVE_MYSQL_MYSQL_H )
	list( APPEND DRIVERS mysql )
	if ( "${DEFAULT_DRIVER}" STREQUAL "" )
		set( DEFAULT_DRIVER "MySQL" )
	endif()
	check_symbol_exists( MYSQL_AUTODETECT_CHARSET_NAME mysql/mysql.h HAVE_DECL_MYSQL_AUTODETECT_CHARSET_NAME )
endif ( HAVE_MYSQL_MYSQL_H )
check_include_file_cxx( ibase.h HAVE_IBASE_H )
if ( HAVE_IBASE_H )
	list( APPEND DRIVERS firebird )
	if ( "${DEFAULT_DRIVER}" STREQUAL "" )
		set( DEFAULT_DRIVER "Firebird" )
	endif()
endif ( HAVE_IBASE_H )
set( CMAKE_REQUIRED_INCLUDES_SAVE "${CMAKE_REQUIRED_INCLUDES}" )
set( CMAKE_REQUIRED_INCLUDES "$ENV{ORACLE_HOME}/rdbms/public" )
check_include_file_cxx( oci.h HAVE_OCI_H )
if ( HAVE_OCI_H )
	list( APPEND DRIVERS oracle )
	if ( "${DEFAULT_DRIVER}" STREQUAL "" )
		set( DEFAULT_DRIVER "Oracle" )
	endif()
	include_directories( "$ENV{ORACLE_HOME}/rdbms/public" )
endif ( HAVE_OCI_H )
set( CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES_SAVE}" )
if ( NOT ( "$ENV{ORACLE_SID}" STREQUAL "" ) )
	set( ORACLE_SID "$ENV{ORACLE_SID}" )
endif()
set( SERIAL_DEVICE "null" )
file( GLOB CUAA0 /dev/cuaa0 )
if ( "${CUAA0}" STREQUAL "/dev/cuaa0" )
	set( SERIAL_DEVICE "cuaa0" )
endif()
file( GLOB TTYS0 /dev/ttyS0 )
if ( "${TTYS0}" STREQUAL "/dev/ttyS0" )
	set( SERIAL_DEVICE "ttyS0" )
endif()
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::sqrtl( 0. ); return ( 0 ); }" HAVE_DECL_SQRTL )
check_cxx_source_compiles( "#include <cmath>
	int main( int, char** ) { ::powl( 0., 0. ); return ( 0 ); }" HAVE_POWL )
check_cxx_source_compiles( "#include <cmath>
	int main( int, char** ) { ::floorl( 0. ); return ( 0 ); }" HAVE_DECL_FLOORL )


include( env )

set( CMAKE_REQUIRED_INCLUDES )
set( CMAKE_REQUIRED_LIBRARIES )
set( CMAKE_REQUIRED_DEFINITIONS )

configure_file( ${CMAKE_HOME_DIRECTORY}/_aux/cmake/config.hxx ${TARGET_PATH}/config.hxx )
configure_file( ${CMAKE_HOME_DIRECTORY}/yaalrc.in ${TARGET_PATH}/yaalrc @ONLY )
configure_file( ${CMAKE_HOME_DIRECTORY}/yaal.pc.in ${TARGET_PATH}/yaal.pc @ONLY )

function( yaal_make_component name )
	msg( "Processing component: ${name}" )
	file( GLOB_RECURSE SRCS RELATIVE ${CMAKE_HOME_DIRECTORY} ${CMAKE_HOME_DIRECTORY}/${name}/*.cxx )
	file( GLOB_RECURSE HDRS RELATIVE ${CMAKE_HOME_DIRECTORY} ${CMAKE_HOME_DIRECTORY}/${name}/*.hxx )
	file( GLOB DRIVER RELATIVE ${CMAKE_HOME_DIRECTORY} ${CMAKE_HOME_DIRECTORY}/${name}/*_driver.cxx )
	list( REMOVE_ITEM SRCS ${DRIVER} dummy_item )
	add_library( ${name} SHARED ${SRCS} ${HDRS} )
	set( HEADERS ${HEADERS} ${HDRS} PARENT_SCOPE )
	set_target_properties(
		${name} PROPERTIES
		OUTPUT_NAME yaal_${name}${LIB_INFIX}
		LINKER_LANGUAGE CXX VERSION ${PROJECT_VERSION}.${PROJECT_SUBVERSION}
		SOVERSION ${PROJECT_VERSION}.${PROJECT_SUBVERSION}
	)
	if ( CMAKE_HOST_WIN32 )
		string( TOUPPER ${name} NAME )
		add_library( ${name}-static STATIC ${SRCS} ${HDRS} )
		set_target_properties( ${name}-static PROPERTIES OUTPUT_NAME yaal_${name} PREFIX "lib" COMPILE_DEFINITIONS __YAAL_${NAME}_BUILD__ )
		add_dependencies( ${name} ${name}-static )
		add_custom_command(
			TARGET ${name}-static
			POST_BUILD COMMAND dumpbin /linkermember:1 build/${CMAKE_BUILD_TYPE}/lib${name}.lib /out:build/${CMAKE_BUILD_TYPE}/${name}${LIB_INFIX}.sym
		)
		add_custom_command(
			TARGET ${name}-static
			POST_BUILD COMMAND makedef --source build/${CMAKE_BUILD_TYPE}/${name}${LIB_INFIX}.sym
				--destination build/${CMAKE_BUILD_TYPE}/${name}${LIB_INFIX}.def --exclude _aux/msvcxx/exclude.sym
		)
		set_target_properties(
			${name} PROPERTIES
			LINK_FLAGS "/DEF:build/${CMAKE_BUILD_TYPE}/${name}${LIB_INFIX}.def /IGNORE:4102"
			COMPILE_DEFINITIONS __YAAL_${NAME}_BUILD__
		)
	endif ( CMAKE_HOST_WIN32 )
endfunction( yaal_make_component )

function( yaal_make_driver name )
	msg( "Processing driver: ${name}" )
	add_library( ${name} MODULE dbwrapper/${name}_driver.cxx )
	set_target_properties(
		${name} PROPERTIES
		OUTPUT_NAME yaal_${name}_driver${LIB_INFIX}
		LINKER_LANGUAGE CXX
		VERSION ${PROJECT_VERSION}.${PROJECT_SUBVERSION}
		SOVERSION ${PROJECT_VERSION}.${PROJECT_SUBVERSION}
	)
endfunction( yaal_make_driver )

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

set( RUNTIME_DESTINATION lib )
set( HEADER_TARGET "${TARGET_PATH}/include/yaal/yaal.hxx" )

if ( CMAKE_HOST_WIN32 )
	set( RUNTIME_DESTINATION bin )
	add_dependencies( hcore msvcxx )
	add_dependencies( hcore-static msvcxx )
	target_link_libraries( hcore msvcxx libintl pthreadVCE2d libeay32 ssleay32 Dbghelp regex libgw32c libgcc Ws2_32 Psapi )
	target_link_libraries( tools hcore libxml2 libxslt libexslt zlib iconv libintl libgw32c libgcc Ws2_32 )
	target_link_libraries( dbwrapper tools hcore libgcc )
	target_link_libraries( hconsole tools hcore curses libintl libgcc )
	target_link_libraries( hdata hconsole dbwrapper tools hcore libgcc )
	msg( "Processing component: msvcxx" )
	file( GLOB SRCS ${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/*.cxx )
	file( GLOB HDRS ${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/*.hxx ${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/*.h ${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/*/*.h ${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/csignal )
	add_library( msvcxx STATIC ${SRCS} ${HDRS} ${CMAKE_HOME_DIRECTORY}/_aux/cmake-config.hxx )
	add_custom_command(
		TARGET hcore-static
		POST_BUILD COMMAND dumpbin /linkermember:1 build/${CMAKE_BUILD_TYPE}/msvcxx.lib /out:build/${CMAKE_BUILD_TYPE}/msvcxx.sym
	)
	add_custom_command(
		TARGET hcore-static
		POST_BUILD COMMAND makedef
			--source build/${CMAKE_BUILD_TYPE}/msvcxx.sym
			--destination build/${CMAKE_BUILD_TYPE}/hcore${LIB_INFIX}.def
			--exclude _aux/msvcxx/exclude.sym
			--append true
	)
	set_target_properties( msvcxx PROPERTIES LINKER_LANGUAGE CXX COMPILE_DEFINITIONS __YAAL_HCORE_BUILD__ )
	add_executable( makedef ${CMAKE_HOME_DIRECTORY}/_aux/makedef.cxx )
	add_executable( mkheaders ${CMAKE_HOME_DIRECTORY}/_aux/mkheaders.cxx )
	add_dependencies( msvcxx makedef )
	set_target_properties( hcore PROPERTIES LINK_INTERFACE_LIBRARIES "" )
	set_target_properties( makedef PROPERTIES COMPILE_DEFINITIONS "YAAL_MSVCXX_FIX_HXX_INCLUDED=1" )
	set_target_properties( mkheaders PROPERTIES COMPILE_DEFINITIONS "YAAL_MSVCXX_FIX_HXX_INCLUDED=1" )
	add_custom_command(
		OUTPUT ${HEADER_TARGET}
		COMMAND mkheaders ${CMAKE_HOME_DIRECTORY} ${TARGET_PATH} "${HEADERS}"
		MAIN_DEPENDENCY ${TARGET_PATH}/config.hxx
		DEPENDS mkheaders ${HEADERS}
	)
else ( CMAKE_HOST_WIN32 )
	target_link_libraries( hcore ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${HCORE_LIBS} nsl )
	target_link_libraries( tools hcore ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} exslt dl z )
	target_link_libraries( dbwrapper tools hcore )
	target_link_libraries( hconsole tools hcore ncurses gpm )
	target_link_libraries( hdata hconsole dbwrapper tools hcore )
	add_custom_command(
		OUTPUT ${HEADER_TARGET}
		COMMAND ./_aux/mkheaders
			--lib-name=${CMAKE_PROJECT_NAME}
			--lib-version=${PACKAGE_VERSION}
			--dir-root=${CMAKE_HOME_DIRECTORY}
			--dir-build=${TARGET_PATH}
			--headers='${HEADERS}'
		MAIN_DEPENDENCY ${TARGET_PATH}/config.hxx
		DEPENDS _aux/mkheaders ${HEADERS}
	)
endif ( CMAKE_HOST_WIN32 )

add_custom_target( headers ALL DEPENDS ${HEADER_TARGET} )

if ( NOT CMAKE_HOST_WIN32 )
	add_definitions( -DHAVE_CONFIG_H )
endif ( NOT CMAKE_HOST_WIN32 )

if ( HAVE_SQLITE3_H )
	target_link_libraries( sqlite3 hcore )
	if ( CMAKE_HOST_WIN32 )
		set_target_properties( sqlite3 PROPERTIES LINK_FLAGS "sqlite3.lib" )
	else()
		target_link_libraries( sqlite3 -lsqlite3 )
	endif()
endif()

if ( HAVE_LIBPQ_FE_H OR HAVE_POSTGRESQL_LIBPQ_FE_H )
	if ( CMAKE_HOST_WIN32 )
	else()
		target_link_libraries( postgresql pq dbwrapper hcore )
	endif()
endif()

if ( HAVE_MYSQL_MYSQL_H )
	if ( CMAKE_HOST_WIN32 )
	else()
		target_link_libraries( mysql -lmysqlclient hcore )
	endif()
endif()

if ( HAVE_IBASE_H )
	if ( CMAKE_HOST_WIN32 )
	else()
		target_link_libraries( firebird fbclient hcore )
	endif()
endif()

if ( HAVE_OCI_H )
	if ( CMAKE_HOST_WIN32 )
	else()
		set_target_properties( oracle PROPERTIES LINK_FLAGS "-L$ENV{ORACLE_HOME}/lib" )
		target_link_libraries( oracle -lclntsh dbwrapper hcore )
	endif()
endif()

if ( CMAKE_HOST_WIN32 )
	install( TARGETS ${COMPONENTS} ${DRIVERS} RUNTIME DESTINATION ${RUNTIME_DESTINATION} LIBRARY DESTINATION bin ARCHIVE DESTINATION lib )
else()
	install( TARGETS ${COMPONENTS} ${DRIVERS} RUNTIME DESTINATION ${RUNTIME_DESTINATION} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib )
endif()

install( DIRECTORY ${TARGET_PATH}/include/yaal DESTINATION include )
install( FILES ${TARGET_PATH}/yaalrc DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR} )
install( FILES ${TARGET_PATH}/yaal.pc DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/pkgconfig )

