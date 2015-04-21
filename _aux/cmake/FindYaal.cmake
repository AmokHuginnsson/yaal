# - Find Yaal
# Find the native Yaal includes and library
#
#  Yaal_INCLUDE_DIRS - where to find Yaal.hxx, etc.
#  Yaal_LIBRARIES    - List of libraries when using Yaal.
#  Yaal_FOUND        - True if Yaal found.


if ( Yaal_INCLUDE_DIR )
  # Already in cache, be silent
  set( Yaal_FIND_QUIETLY TRUE )
endif ( Yaal_INCLUDE_DIR )

if ( NOT ( "$ENV{PREFIX}" STREQUAL "" ) )
	set( _Yaal_INCLUDE_SEARCH_DIRS $ENV{PREFIX}/include )
	set( _Yaal_LIBRARIES_SEARCH_DIRS $ENV{PREFIX}/lib )
else()
	set( _Yaal_INCLUDE_SEARCH_DIRS /usr/local/include )
	set( _Yaal_LIBRARIES_SEARCH_DIRS /usr/local/lib )
endif()

find_path( Yaal_INCLUDE_DIR yaal/yaal.hxx HINTS ${_Yaal_INCLUDE_SEARCH_DIRS} )

foreach( component ${Yaal_FIND_COMPONENTS} )
	string( TOUPPER ${component} COMPONENT )
	find_library( Yaal_LIBRARY_${COMPONENT} NAMES yaal_${component}${LIB_INFIX} HINTS ${_Yaal_LIBRARIES_SEARCH_DIRS} )
	mark_as_advanced( Yaal_LIBRARY_${COMPONENT} )
	if ( NOT ( "${Yaal_LIBRARY_${COMPONENT}}" STREQUAL "Yaal_LIBRARY_${COMPONENT}-NOTFOUND" ) )
		set( Yaal_LIBRARY ${Yaal_LIBRARY} yaal_${component}${LIB_INFIX} )
	endif()
endforeach()

mark_as_advanced( Yaal_LIBRARY Yaal_INCLUDE_DIR )

# Per-recommendation
set( Yaal_INCLUDE_DIRS "${Yaal_INCLUDE_DIR}" )
set( Yaal_LIBRARIES    "${Yaal_LIBRARY}" )

# handle the QUIETLY and REQUIRED arguments and set Yaal_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Yaal DEFAULT_MSG Yaal_LIBRARIES Yaal_INCLUDE_DIRS )

