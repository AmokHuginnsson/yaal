cmake_minimum_required( VERSION 2.6 )
project( yaal )
set( COMPONENTS hcore tools dbwrapper hconsole hdata )
include_directories( "${yaal_SOURCE_DIR}" )

macro( yaal_make_component name )
	file( GLOB SRCS ${yaal_SOURCE_DIR}/${name}/*.cxx )
	add_library( ${name} SHARED ${SRCS} )
	set_target_properties( ${name} PROPERTIES LINKER_LANGUAGE CXX )
endmacro( yaal_make_component )

foreach( COMPONENT ${COMPONENTS} )
	yaal_make_component( "${COMPONENT}" )
endforeach( COMPONENT )
