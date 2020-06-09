cmake_minimum_required( VERSION 3.4 )

set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "yaal (Yet Another Abstraction Layer) - a general purpose C++ library.")
set( CPACK_WIX_UPGRADE_GUID "436f6465-5374-6174-696f-6e207961616c" )
set( CPACK_RESOURCE_FILE_WELCOME "${CMAKE_HOME_DIRECTORY}/doc/PROGRAMMER.READ.ME" )
set( CPACK_RESOURCE_FILE_README "${CMAKE_HOME_DIRECTORY}/doc/READ.ME.FIRST.OR.DIE" )
set( CPACK_COMPONENTS_ALL runtime dependencies configuration devel )

include( common )

option( YAAL_AUTO_SANITY "Enable automatic environment sanitization." OFF )

if ( DEFINED BUILD_PACKAGE )
	set( YAAL_AUTO_SANITY true )
endif()

include( types )

if ( CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" )
	set( TARGET_CPU_BITS 64 )
else()
	set( TARGET_CPU_BITS 32 )
endif()

add_definitions( -D__YAAL_BUILD__ )
if ( CMAKE_HOST_WIN32 )
	add_definitions( -D__MSVCXX__ /FI_aux/msvcxx/fix.hxx )
	include_directories( "${CMAKE_INCLUDE_PATH}/libxml2" )
	set( LIB_PREFIX "" )
	set( LIB_EXT "dll" )
	set( SYMBOL_PREFIX "" )
	set( EXE_SUFFIX ".exe" )
	set( CMAKE_REQUIRED_INCLUDES "${CMAKE_HOME_DIRECTORY}/_aux/msvcxx; ${CMAKE_INCLUDE_PATH}; ${CMAKE_HOME_DIRECTORY}" )
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
cxx_search_libraries( HCORE_LIBS pcre_compile2 pcre )

check_include_file_cxx( execinfo.h HAVE_EXECINFO_H )
check_include_file_cxx( termio.h HAVE_TERMIO_H )
check_include_file_cxx( tty.h HAVE_TTY_H )
check_include_file_cxx( sys/ioctl.h HAVE_SYS_IOCTL_H )
check_include_file_cxx( sys/utime.h HAVE_SYS_UTIME_H )
check_include_file_cxx( utime.h HAVE_UTIME_H )
check_include_file_cxx( sys/consio.h HAVE_SYS_CONSIO_H )
check_include_file_cxx( gpm.h HAVE_GPM_H )
check_include_file_cxx( netdb.h HAVE_NETDB_H )
check_include_file_cxx( arpa/inet.h HAVE_ARPA_INET_H )
check_include_file_cxx( netinet/in.h HAVE_NETINET_IN_H )
check_include_file_cxx( sys/un.h HAVE_SYS_UN_H )
check_include_file_cxx( ncurses/curses.h HAVE_NCURSES_CURSES_H )
check_include_file_cxx( libgen.h HAVE_LIBGEN_H )
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
check_function_exists( strtold HAVE_DECL_STRTOLD )
check_function_exists( pthread_setname_np HAVE_PTHREAD_SETNAME_NP )
check_function_exists( pthread_set_name_np HAVE_PTHREAD_SET_NAME_NP )
check_function_exists( prctl HAVE_PRCTL )
check_function_exists( gethostbyname_r HAVE_GETHOSTBYNAME_R )
check_function_exists( gethostbyaddr_r HAVE_GETHOSTBYADDR_R )
if ( CMAKE_HOST_WIN32 )
	set( CMAKE_REQUIRED_LIBRARIES Ws2_32 )
	check_symbol_exists( getaddrinfo Ws2tcpip.h HAVE_GETADDRINFO )
	check_symbol_exists( getnameinfo Ws2tcpip.h HAVE_GETNAMEINFO )
	set( CMAKE_REQUIRED_LIBRARIES )
else()
	check_function_exists( getaddrinfo HAVE_GETADDRINFO )
	check_function_exists( getnameinfo HAVE_GETNAMEINFO )
endif()
if ( CMAKE_HOST_WIN32 )
	find_library( WINPTHREAD NAMES winpthread libwinpthread )
	set( CMAKE_REQUIRED_INCLUDES_SAVE "${CMAKE_REQUIRED_INCLUDES}" )
	set( CMAKE_REQUIRED_INCLUDES pthread.h )
	set( CMAKE_REQUIRED_LIBRARIES "${WINPTHREAD}" )
	check_function_exists( clock_gettime HAVE_CLOCK_GETTIME )
	set( CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES_SAVE}" )
	set( CMAKE_REQUIRED_LIBRARIES )
else()
	check_function_exists( clock_gettime HAVE_CLOCK_GETTIME )
endif()
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
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::expl( 0. ); return ( 0 ); }" HAVE_DECL_EXPL )
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::logl( 0. ); return ( 0 ); }" HAVE_DECL_LOGL )
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::sinl( 0. ); return ( 0 ); }" HAVE_DECL_SINL )
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::cosl( 0. ); return ( 0 ); }" HAVE_DECL_COSL )
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::tanl( 0. ); return ( 0 ); }" HAVE_DECL_TANL )
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::asinl( 0. ); return ( 0 ); }" HAVE_DECL_ASINL )
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::acosl( 0. ); return ( 0 ); }" HAVE_DECL_ACOSL )
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::atanl( 0. ); return ( 0 ); }" HAVE_DECL_ATANL )
check_cxx_source_compiles("#include <cmath>
	int main( int, char** ) { ::sincosl( 0., static_cast<double long*>( 0 ), static_cast<double long*>( 0 ) ); return ( 0 ); }" HAVE_DECL_SINCOSL )
check_cxx_source_compiles( "#include <cmath>
	int main( int, char** ) { ::powl( 0., 0. ); return ( 0 ); }" HAVE_POWL )
check_cxx_source_compiles( "#include <cmath>
	int main( int, char** ) { ::fmodl( 0., 0. ); return ( 0 ); }" HAVE_DECL_FMODL )
check_cxx_source_compiles( "#include <cmath>
	int main( int, char** ) { ::floorl( 0. ); return ( 0 ); }" HAVE_DECL_FLOORL )
check_cxx_source_compiles( "#include <cmath>
	int main( int, char** ) { ::ceill( 0. ); return ( 0 ); }" HAVE_DECL_CEILL )
check_cxx_source_compiles( "#include <cmath>
	int main( int, char** ) { ::roundl( 0. ); return ( 0 ); }" HAVE_DECL_ROUNDL )
if ( NOT CMAKE_HOST_WIN32 )
	set( HOME_ENV_VAR "HOME" )
else()
	set( HOME_ENV_VAR "HOMEPATH" )
endif()

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
		LINKER_LANGUAGE CXX
		VERSION ${PROJECT_VERSION}.${PROJECT_SUBVERSION}.${PROJECT_EXTRAVERSION}
		SOVERSION ${PROJECT_VERSION}.${PROJECT_SUBVERSION}
	)
	if ( CMAKE_HOST_WIN32 )
		string( TOUPPER ${name} NAME )
		add_library( ${name}-static STATIC ${SRCS} ${HDRS} )
		target_include_directories( ${name} BEFORE PRIVATE "${CMAKE_HOME_DIRECTORY}/_aux/msvcxx" )
		target_include_directories( ${name}-static BEFORE PRIVATE "${CMAKE_HOME_DIRECTORY}/_aux/msvcxx" )
		set_target_properties( ${name}-static PROPERTIES OUTPUT_NAME yaal_${name} PREFIX "lib" COMPILE_DEFINITIONS __YAAL_${NAME}_BUILD__ )
		add_custom_command(
			OUTPUT ${TARGET_PATH}/${name}${LIB_INFIX}.sym
			COMMAND dumpbin /linkermember:1 ${TARGET_PATH}/libyaal_${name}.lib /out:${TARGET_PATH}/${name}${LIB_INFIX}.sym
			DEPENDS ${name}-static
		)
		add_custom_command(
			OUTPUT ${TARGET_PATH}/${name}${LIB_INFIX}.def
			COMMAND makedef
				--source ${TARGET_PATH}/${name}${LIB_INFIX}.sym
				--destination ${TARGET_PATH}/${name}${LIB_INFIX}.def
				--exclude ${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/exclude.sym
			MAIN_DEPENDENCY ${TARGET_PATH}/${name}${LIB_INFIX}.sym
			DEPENDS makedef
		)
		add_custom_target( ${name}-def DEPENDS ${TARGET_PATH}/${name}${LIB_INFIX}.def )
		add_dependencies( ${name} ${name}-def )
		set_target_properties(
			${name} PROPERTIES
			LINK_FLAGS "/DEF:${TARGET_PATH}/${name}${LIB_INFIX}.def /IGNORE:4102"
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
		VERSION ${PROJECT_VERSION}.${PROJECT_SUBVERSION}.${PROJECT_EXTRAVERSION}
		SOVERSION ${PROJECT_VERSION}.${PROJECT_SUBVERSION}
	)
	if ( CMAKE_HOST_WIN32 )
		target_include_directories( ${name} BEFORE PRIVATE "${CMAKE_HOME_DIRECTORY}/_aux/msvcxx" )
	endif()
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
set( SSL_KEYS_DIR "db/${PROJECT_NAME}/keys/" )
set( SSL_KEYS "${SSL_KEYS_DIR}/key ${SSL_KEYS_DIR}/pem" )

if ( CMAKE_HOST_WIN32 )
	set( RUNTIME_DESTINATION bin )
	add_dependencies( hcore msvcxx )
	add_dependencies( hcore-static msvcxx )
	target_link_libraries( hcore msvcxx libintl libwinpthread.lib iconv libssl libcrypto Dbghelp pcre Ws2_32 Psapi )
	target_link_libraries( tools hcore libxml2 libxslt libexslt zlib libintl Ws2_32 )
	target_link_libraries( dbwrapper tools hcore )
	target_link_libraries( hconsole tools hcore curses libintl )
	target_link_libraries( hdata hconsole dbwrapper tools hcore )
	msg( "Processing component: msvcxx" )
	file( GLOB SRCS ${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/*.cxx )
	file(
		GLOB HDRS
		${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/*.hxx
		${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/*.h
		${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/*/*.h
		${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/csignal
		${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/ctime
		${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/cmath
		${CMAKE_HOME_DIRECTORY}/_aux/yaal.natvis
	)
	add_library( msvcxx STATIC ${SRCS} ${HDRS} ${CMAKE_HOME_DIRECTORY}/_aux/cmake/config.hxx )
	add_custom_command(
		OUTPUT ${TARGET_PATH}/msvcxx.sym
		COMMAND dumpbin /linkermember:1 ${TARGET_PATH}/msvcxx.lib /out:${TARGET_PATH}/msvcxx.sym
		DEPENDS msvcxx
	)
	add_custom_command(
		OUTPUT ${TARGET_PATH}/hcore${LIB_INFIX}.def
		COMMAND makedef
			--source ${TARGET_PATH}/msvcxx.sym
			--destination ${TARGET_PATH}/hcore${LIB_INFIX}.def
			--exclude ${CMAKE_HOME_DIRECTORY}/_aux/msvcxx/exclude.sym
			--append true
		DEPENDS hcore-static ${TARGET_PATH}/msvcxx.sym
		APPEND
	)
	set_target_properties( msvcxx PROPERTIES LINKER_LANGUAGE CXX COMPILE_DEFINITIONS __YAAL_HCORE_BUILD__ )
	add_executable( makedef ${CMAKE_HOME_DIRECTORY}/_aux/makedef.cxx )
	add_executable( mkheaders ${CMAKE_HOME_DIRECTORY}/_aux/mkheaders.cxx )
	add_executable( update-commit-id ${CMAKE_HOME_DIRECTORY}/_aux/update-commit-id.cxx )
	add_executable( mklicense "${CMAKE_HOME_DIRECTORY}/_aux/mklicense.cxx" )
	add_dependencies( msvcxx makedef )
	set_target_properties( hcore PROPERTIES LINK_INTERFACE_LIBRARIES "" )
	set_target_properties( makedef PROPERTIES COMPILE_DEFINITIONS "YAAL_MSVCXX_FIX_HXX_INCLUDED=1" )
	set_target_properties( mkheaders PROPERTIES COMPILE_DEFINITIONS "YAAL_MSVCXX_FIX_HXX_INCLUDED=1" )
	add_custom_command(
		OUTPUT ${HEADER_TARGET}
		COMMAND mkheaders ${CMAKE_HOME_DIRECTORY} ${TARGET_PATH} "${HEADERS}"
		WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
		MAIN_DEPENDENCY ${TARGET_PATH}/config.hxx
		DEPENDS mkheaders ${HEADERS}
	)
else ( CMAKE_HOST_WIN32 )
	target_link_libraries( hcore ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${HCORE_LIBS} nsl )
	target_link_libraries( tools hcore ${LIBXML2_LIBRARIES} ${LIBXSLT_LIBRARIES} exslt dl z )
	target_link_libraries( dbwrapper tools hcore )
	target_link_libraries( hconsole tools hcore ${CURSES_LIBRARIES} gpm )
	target_link_libraries( hdata hconsole dbwrapper tools hcore )
	add_custom_command(
		OUTPUT ${HEADER_TARGET}
		COMMAND ${CMAKE_HOME_DIRECTORY}/_aux/mkheaders
			--lib-name=${CMAKE_PROJECT_NAME}
			--dir-root=${CMAKE_HOME_DIRECTORY}
			--dir-build=${TARGET_PATH}
			--headers='${HEADERS}'
		MAIN_DEPENDENCY ${TARGET_PATH}/config.hxx
		DEPENDS ${CMAKE_HOME_DIRECTORY}/_aux/mkheaders ${HEADERS}
	)
endif ( CMAKE_HOST_WIN32 )

add_custom_command(
	OUTPUT ${SSL_KEYS_DIR}/pem
	COMMAND ${CMAKE_HOME_DIRECTORY}/_aux/gen-keys ${TARGET_PATH}/${SSL_KEYS_DIR}
	DEPENDS ${CMAKE_HOME_DIRECTORY}/_aux/gen-keys
)

add_dependencies( hcore commit_id )

add_custom_target( headers ALL DEPENDS ${HEADER_TARGET} )
add_custom_target( ssl_keys ALL DEPENDS ${SSL_KEYS_DIR}/pem )

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
		find_library( MYSQL_CLIENT_LIBRARY NAMES mysqlclient mariadbclient )
		target_link_libraries( mysql ${MYSQL_CLIENT_LIBRARY} hcore )
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
	install( FILES "${TARGET_PATH}/update-commit-id.exe" COMPONENT devel RENAME "yaal-update-commit-id.exe" DESTINATION ${RUNTIME_DESTINATION} )
	install( FILES "${TARGET_PATH}/mklicense.exe" COMPONENT devel RENAME "yaal-mklicense.exe" DESTINATION ${RUNTIME_DESTINATION} )
	install( TARGETS ${COMPONENTS} ${DRIVERS} RUNTIME DESTINATION ${RUNTIME_DESTINATION} COMPONENT runtime LIBRARY DESTINATION bin COMPONENT runtime )
	install( TARGETS ${COMPONENTS} ARCHIVE DESTINATION lib COMPONENT devel )
	if ( DEFINED BUILD_PACKAGE )
		install( FILES ${TARGET_PATH}/yaalrc DESTINATION etc COMPONENT configuration )
		set( EXTRA_DEPENDENCIES iconv.dll libcrypto-1_1.dll libexslt.dll libiconv-2.dll libintl-2.dll libwinpthread.dll libxml2.dll libxslt.dll pcre.dll sqlite3.dll libssl-1_1.dll zlib1.dll )
		prepend( EXTRA_DEPENDENCIES ${CMAKE_INSTALL_PREFIX}/${RUNTIME_DESTINATION}/ ${EXTRA_DEPENDENCIES} )
		append( DRIVERS_REAL _driver ${DRIVERS} )
		prepend( DEBUG_LIBS build/debug/yaal_ ${COMPONENTS} ${DRIVERS_REAL} )
		append( DEBUG_LIBS -d.dll ${DEBUG_LIBS} )
		prepend( DEBUG_ARCHIVE build/debug/yaal_ ${COMPONENTS} )
		append( DEBUG_ARCHIVE -d.lib ${DEBUG_ARCHIVE} )
		install( FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} ${EXTRA_DEPENDENCIES} DESTINATION bin COMPONENT dependencies )
		install( FILES ${DEBUG_LIBS} DESTINATION ${RUNTIME_DESTINATION} COMPONENT devel )
		install( FILES ${DEBUG_ARCHIVE} DESTINATION lib COMPONENT devel )
		install( FILES ${CMAKE_INSTALL_PREFIX}/include/libintl.h DESTINATION include COMPONENT devel )
		install( FILES ${CMAKE_INSTALL_PREFIX}/lib/libintl.lib DESTINATION lib COMPONENT devel )
		install( DIRECTORY _aux DESTINATION share COMPONENT devel )
		install( FILES ${CMAKE_SOURCE_DIR}/configure.js DESTINATION share/_aux COMPONENT devel )
	endif()
else()
	install( TARGETS ${COMPONENTS} ${DRIVERS} RUNTIME DESTINATION ${RUNTIME_DESTINATION} LIBRARY DESTINATION lib ARCHIVE DESTINATION lib )
	install( DIRECTORY _aux DESTINATION ${datadir}/yaal USE_SOURCE_PERMISSIONS )
endif()

install( DIRECTORY ${TARGET_PATH}/include/yaal DESTINATION include COMPONENT devel )
install( DIRECTORY ${TARGET_PATH}/db DESTINATION ${localstatedir} )
install( FILES ${TARGET_PATH}/yaalrc DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR} )
install( FILES ${TARGET_PATH}/yaal.pc DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/pkgconfig )

#message("CMAKE_HOME_DIRECTORY = ${CMAKE_HOME_DIRECTORY}")
#message("TARGET_PATH = ${TARGET_PATH}")
#message("CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
#message("CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
#message( "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )

