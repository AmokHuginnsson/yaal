cmake_minimum_required( VERSION 2.8 )
string( TOUPPER ${PROJECT_NAME} PROJECT_NAME_UC )
if ( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE debug CACHE STRING "Choose the type of build, options are: debug, release." FORCE )
endif ( NOT CMAKE_BUILD_TYPE )
if ( ( NOT ( "${CMAKE_BUILD_TYPE}" STREQUAL "debug" ) ) AND ( NOT ( "${CMAKE_BUILD_TYPE}" STREQUAL "release" ) ) )
	message( FATAL_ERROR "Only `debug' and `release' tergets are supported." )
endif ( ( NOT ( "${CMAKE_BUILD_TYPE}" STREQUAL "debug" ) ) AND ( NOT ( "${CMAKE_BUILD_TYPE}" STREQUAL "release" ) ) )
if ( ${CMAKE_HOME_DIRECTORY} MATCHES "/_aux" )
	get_filename_component( CMAKE_HOME_DIRECTORY ${CMAKE_HOME_DIRECTORY}/../ ABSOLUTE )
endif()

if ( NOT ( "$ENV{PREFIX}" STREQUAL "" ) )
	set( CMAKE_INSTALL_PREFIX $ENV{PREFIX} CACHE PATH "Install prefix." FORCE )
endif ( NOT ( "$ENV{PREFIX}" STREQUAL "" ) )

include( CheckCXXSourceCompiles )
include( CheckCXXCompilerFlag )
include( CheckIncludeFileCXX )
include( CheckFunctionExists )
include( CheckSymbolExists )

macro( use_cxx_compiler_flag flag )
	string( REGEX REPLACE "^-" "" name "${flag}" )
	string( REGEX REPLACE "\\+\\+" "XX" name ${name} )
	check_cxx_compiler_flag( ${flag} ${name} )
#	message( "FLAG = ${flag}, NAME = ${name}, VALUE = ${${name}}" )
	if ( ${${name}} )
		add_definitions( ${flag} )
	endif( ${${name}} )
endmacro( use_cxx_compiler_flag )

set( V ${VERBOSE} )
unset( VERBOSE CACHE )
set( VERBOSE ${V} )

macro( msg text )
	if ( ${VERBOSE} )
		message( STATUS ${text} )
	endif( ${VERBOSE} )
endmacro( msg )

use_cxx_compiler_flag( -fmessage-length=0 )
use_cxx_compiler_flag( -std=gnu++98 )
use_cxx_compiler_flag( -pipe )
use_cxx_compiler_flag( -m64 )
use_cxx_compiler_flag( -Wl,--fatal-warnings )
use_cxx_compiler_flag( -Wl,--no-undefined )
use_cxx_compiler_flag( -Wl,--enable-auto-import )
use_cxx_compiler_flag( -Wextra )
use_cxx_compiler_flag( -W )
use_cxx_compiler_flag( -Wall )
use_cxx_compiler_flag( -Wcast-align )
use_cxx_compiler_flag( -Wcast-qual )
use_cxx_compiler_flag( -Wconversion  )
use_cxx_compiler_flag( -Wendif-labels )
use_cxx_compiler_flag( -Werror )
use_cxx_compiler_flag( -Winline )
use_cxx_compiler_flag( -Wlarger-than-16384 )
use_cxx_compiler_flag( -Wlong-long )
use_cxx_compiler_flag( -Wmissing-noreturn )
use_cxx_compiler_flag( -Wpointer-arith )
use_cxx_compiler_flag( -Wredundant-decls )
use_cxx_compiler_flag( -Wsign-compare )
use_cxx_compiler_flag( -Wundef )
use_cxx_compiler_flag( -Wwrite-strings )
use_cxx_compiler_flag( -Weffc++ )
use_cxx_compiler_flag( -Wsign-promo )

if ( "${CMAKE_BUILD_TYPE}" STREQUAL "debug" )
	msg( "Generating debug build." )
	set( LIB_INFIX "-d" )
	use_cxx_compiler_flag( -O0 )
	use_cxx_compiler_flag( -g3 )
	use_cxx_compiler_flag( -ggdb3 )
	use_cxx_compiler_flag( -fno-inline )
	add_definitions( -D__DEBUG__ )
else ( "${CMAKE_BUILD_TYPE}" STREQUAL "release" )
	msg( "Generating release build." )
	set( LIB_INFIX "" )
	use_cxx_compiler_flag( -O3 )
	use_cxx_compiler_flag( -fexpensive-optimizations )
	use_cxx_compiler_flag( -Wno-error )
	use_cxx_compiler_flag( -DNDEBUG )
endif ( "${CMAKE_BUILD_TYPE}" STREQUAL "debug" )

add_definitions( -D__ID__="" -D__TID__="" )
include_directories( build/${CMAKE_BUILD_TYPE} )
#set( CMAKE_VERBOSE_MAKEFILE true )
enable_language( C )

if ( CMAKE_HOST_WIN32 )
	add_definitions( -D__MSVCXX__ -D_GNU_SOURCE )
	set_property( DIRECTORY APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_HOME_DIRECTORY}/_aux/msvcxx;$(VCInstallDir)/include;$ENV{CMAKE_INCLUDE_PATH}; $ENV{CMAKE_INCLUDE_PATH}/glibc; $ENV{CMAKE_INCLUDE_PATH}/winx;${CMAKE_HOME_DIRECTORY}" )
	link_directories( $ENV{CMAKE_LIBRARY_PATH} )
	set( LIB_PREFIX "" )
	set( LIB_EXT "dll" )
	set( SYMBOL_PREFIX "" )
else ( CMAKE_HOST_WIN32 )
	include_directories( ${CMAKE_HOME_DIRECTORY} )
	set( LIB_PREFIX "lib" )
	set( LIB_EXT "so" )
	set( SYMBOL_PREFIX "" )
endif ( CMAKE_HOST_WIN32 )
if ( CMAKE_HOST_WIN32 )
	set( CMAKE_REQUIRED_INCLUDES "${CMAKE_HOME_DIRECTORY}/_aux/msvcxx;\\\\.\\$(VCInstallDir)/include;$ENV{CMAKE_INCLUDE_PATH}; $ENV{CMAKE_INCLUDE_PATH}/glibc; $ENV{CMAKE_INCLUDE_PATH}/winx;${CMAKE_HOME_DIRECTORY}" )
	set( CMAKE_REQUIRED_LIBRARIES "$ENV{CMAKE_LIBRARY_PATH}/libgw32c.a" )
	set( CMAKE_REQUIRED_DEFINITIONS -D__value=value_ )
endif ( CMAKE_HOST_WIN32 )
