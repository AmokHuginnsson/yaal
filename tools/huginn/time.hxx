/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_TIME_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_TIME_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "hcore/htime.hxx"

namespace yaal {

namespace tools {

namespace huginn {

typedef yaal::hcore::HTime& ( yaal::hcore::HTime::*time_mod_t )( int );
typedef yaal::hcore::HTime& ( yaal::hcore::HTime::*time_set_t )( int, int, int );
typedef int ( yaal::hcore::HTime::*time_get_t )( void ) const;

class HTime : public HHuginn::HValue {
	yaal::hcore::HTime _time;
public:
	HTime( HHuginn::HClass const*, yaal::hcore::HTime const& );
	static HHuginn::value_t mod( char const*, time_mod_t, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t get( char const*, time_get_t, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t set( char const*, time_set_t, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t set_datetime( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t get_month( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t subtract( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t from_string( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t to_string( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime* );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_TIME_HXX_INCLUDED */

