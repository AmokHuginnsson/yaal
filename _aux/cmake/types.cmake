check_type_size( "int short" INT_SHORT )
if ( HAVE_INT_SHORT )
	set( SIZEOF_INT_SHORT ${INT_SHORT} )
endif()
check_type_size( "int" INT )
if ( HAVE_INT )
	set( SIZEOF_INT ${INT} )
endif()
check_type_size( "int long" INT_LONG )
if ( HAVE_INT_LONG )
	set( SIZEOF_INT_LONG ${INT_LONG} )
endif()
check_type_size( "int long long" INT_LONG_LONG )
if ( HAVE_INT_LONG_LONG )
	set( SIZEOF_INT_LONG_LONG ${INT_LONG_LONG} )
endif()
check_type_size( "float" FLOAT )
if ( HAVE_FLOAT )
	set( SIZEOF_FLOAT ${FLOAT} )
endif()
check_type_size( "double" DOUBLE )
if ( HAVE_DOUBLE )
	set( SIZEOF_DOUBLE ${DOUBLE} )
endif()
check_type_size( "double long" DOUBLE_LONG )
if ( HAVE_DOUBLE_LONG )
	set( SIZEOF_DOUBLE_LONG ${DOUBLE_LONG} )
endif()
check_type_size( "void*" VOIDP )
if ( HAVE_VOIDP )
	set( SIZEOF_VOIDP ${VOIDP} )
endif()
if ( CMAKE_HOST_WIN32 )
	set ( CMAKE_REQUIRED_DEFINITIONS_SAVE "${CMAKE_REQUIRED_DEFINITIONS}" )
	set ( CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS} -D_USE_32BIT_TIME_T" )
endif()
check_type_size( "time_t" TIME_T )
if ( CMAKE_HOST_WIN32 )
	set ( CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS_SAVE}" )
endif()
if ( HAVE_TIME_T )
	set( SIZEOF_TIME_T ${TIME_T} )
endif()
if ( CMAKE_HOST_WIN32 )
	check_type_size( "_ino_t" INO_T )
	if ( HAVE_INO_T )
		set( SIZEOF_INO_T ${INO_T} )
	endif()
endif()
