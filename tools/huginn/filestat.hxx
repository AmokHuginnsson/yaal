/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_FILESTAT_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_FILESTAT_HXX_INCLUDED 1

#include "tools/filesystem.hxx"
#include "tools/hfsitem.hxx"
#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

typedef yaal::hcore::HTime ( HFSItem::* time_getter_t )( void ) const;

class HFileStat : public HValue {
private:
	HClass const* _fileTypeClass;
	HClass const* _exceptionClass;
	filesystem::path_t _path;
public:
	HFileStat( HClass const*, HClass const*, HClass const*, filesystem::path_t const& );
	static HHuginn::value_t id( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t name( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t size( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t mode( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t type( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t user( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t group( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t get_times( char const*, time_getter_t, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime*, HClass const* );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override __attribute__((noreturn));
	HFileStat( HFileStat const& ) = delete;
	HFileStat& operator = ( HFileStat const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FILESTAT_HXX_INCLUDED */

