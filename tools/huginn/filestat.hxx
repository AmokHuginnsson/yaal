/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  numbersetstatistics.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_FILESTAT_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_FILESTAT_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/filesystem.hxx"
#include "tools/hfsitem.hxx"

namespace yaal {

namespace tools {

namespace huginn {

typedef yaal::hcore::HTime ( HFSItem::* time_getter_t )( void ) const;

class HFileStat : public HHuginn::HValue {
private:
	HHuginn::HClass const* _exceptionClass;
	HHuginn::HClass const* _timeClass;
	filesystem::path_t _path;
public:
	HFileStat( HHuginn::HClass const*, HHuginn::HClass const*, HHuginn::HClass const*, filesystem::path_t const& );
	static HHuginn::value_t id( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t name( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t size( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t mode( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t type( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t user( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t group( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t get_times( char const*, time_getter_t, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime* );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, int ) const override __attribute__((noreturn));
	HFileStat( HFileStat const& ) = delete;
	HFileStat& operator = ( HFileStat const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FILESTAT_HXX_INCLUDED */

