cmake_minimum_required( VERSION 3.4 )

include( CheckCXXCompilerFlag )

function( use_cxx_compiler_flag flag )
	string( REGEX REPLACE "[,=-]+" "" name "${flag}" )
	string( REGEX REPLACE "\\+\\+" "XX" name ${name} )
	if ( ${flag} MATCHES "-Wl,.*" )
		set( CMAKE_REQUIRED_FLAGS "-Werror ${flag} -pedantic-errors" )
	endif()
	check_cxx_compiler_flag( ${flag} ${name} )
#	message( "FLAG = ${flag}, NAME = ${name}, VALUE = ${${name}}" )
	if ( ${${name}} )
		if ( NOT ( ${flag} MATCHES "-Wl,.*" ) )
			add_compile_options( ${flag} )
		else()
			set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${flag}" PARENT_SCOPE )
			set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${flag}" PARENT_SCOPE )
			set( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${flag}" PARENT_SCOPE )
		endif()
	endif( ${${name}} )
endfunction( use_cxx_compiler_flag )

include( CheckFunctionExists )
include( CheckLibraryExists )

macro ( cxx_search_libraries libs func )
	string( TOUPPER ${func} FUNC )
	check_function_exists( ${func} HAVE_${FUNC} )
	if ( NOT HAVE_${FUNC} )
		foreach ( lib ${ARGN} )
			string( TOUPPER ${lib} LIB )
			set( CMAKE_C_COMPILER_LOADED )
			check_library_exists( ${lib} ${func} "" HAVE_${LIB} )
			if ( HAVE_${LIB} )
				set ( HAVE_${FUNC} true )
				set ( ${FUNC}_LIB "${lib}" CACHE INTERNAL "Library providing ${func}" )
				set ( ${libs} ${${libs}} "${lib}" )
				break()
			endif()
		endforeach()
	endif()
endmacro ( cxx_search_libraries )

function( msg text )
	if ( ${VERBOSE} )
		message( STATUS ${text} )
	endif( ${VERBOSE} )
endfunction( msg )

macro ( today RESULT )
	if (WIN32)
		try_run( RUN_RESULT COMPILE_RESULT ${CMAKE_HOME_DIRECTORY}/build ${CMAKE_HOME_DIRECTORY}/../yaal/_aux/cmake/getdate.cxx RUN_OUTPUT_VARIABLE ${RESULT} )
	elseif(UNIX)
		execute_process( COMMAND "date" "+%Y%m%d" OUTPUT_VARIABLE ${RESULT} OUTPUT_STRIP_TRAILING_WHITESPACE )
	else (WIN32)
		message( SEND_ERROR "date not implemented" )
		set(${RESULT} 000000)
	endif (WIN32)
endmacro ( today )

function( prepend var prefix )
	set( listVar "" )
	foreach( f ${ARGN} )
		list( APPEND listVar "${prefix}${f}" )
	endforeach()
	set( ${var} "${listVar}" PARENT_SCOPE )
endfunction()

function( append var suffix )
	set( listVar "" )
	foreach( f ${ARGN} )
		list( APPEND listVar "${f}${suffix}" )
	endforeach()
	set( ${var} "${listVar}" PARENT_SCOPE )
endfunction()

