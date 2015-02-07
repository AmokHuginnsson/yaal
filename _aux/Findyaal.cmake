# - Find yaal
# Find the native yaal includes and library
#
#  yaal_INCLUDE_DIRS - where to find yaal.hxx, etc.
#  yaal_LIBRARIES    - List of libraries when using yaal.
#  yaal_FOUND        - True if yaal found.


if ( yaal_INCLUDE_DIR )
  # Already in cache, be silent
  set( yaal_FIND_QUIETLY TRUE )
endif ( yaal_INCLUDE_DIR )

if ( NOT ( "$ENV{PREFIX}" STREQUAL "" ) )
	set( _yaal_INCLUDE_SEARCH_DIRS $ENV{PREFIX}/include )
	set( _yaal_LIBRARIES_SEARCH_DIRS $ENV{PREFIX}/lib )
else()
	set( _yaal_INCLUDE_SEARCH_DIRS /usr/local/include )
	set( _yaal_LIBRARIES_SEARCH_DIRS /usr/local/lib )
endif()

find_path( yaal_INCLUDE_DIR yaal/yaal.hxx HINTS ${_yaal_INCLUDE_SEARCH_DIRS} )

foreach( component ${yaal_FIND_COMPONENTS} )
	string( TOUPPER ${component} COMPONENT )
	find_library( yaal_LIBRARY_${COMPONENT} NAMES yaal_${component}${LIB_INFIX} HINTS ${_yaal_LIBRARIES_SEARCH_DIRS} )
	mark_as_advanced( yaal_LIBRARY_${COMPONENT} )
	if ( NOT ( "${yaal_LIBRARY_${COMPONENT}}" STREQUAL "yaal_LIBRARY_${COMPONENT}-NOTFOUND" ) )
		set( yaal_LIBRARY ${yaal_LIBRARY} yaal_${component}${LIB_INFIX} )
	endif()
endforeach()

mark_as_advanced( yaal_LIBRARY yaal_INCLUDE_DIR )

# Per-recommendation
set( yaal_INCLUDE_DIRS "${yaal_INCLUDE_DIR}" )
set( yaal_LIBRARIES    "${yaal_LIBRARY}" )

# handle the QUIETLY and REQUIRED arguments and set yaal_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( yaal DEFAULT_MSG yaal_LIBRARIES yaal_INCLUDE_DIRS )

