/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_DBWRAPPER_HUGINN_DATABASECONNECTION_HXX_INCLUDED
#define YAAL_DBWRAPPER_HUGINN_DATABASECONNECTION_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "hcore/hresource.hxx"
#include "dbwrapper/hdatabase.hxx"

namespace yaal {

namespace dbwrapper {

namespace huginn {

class HDatabaseConnection : public tools::HHuginn::HValue {
public:
private:
	yaal::dbwrapper::database_ptr_t _database;
public:
	HDatabaseConnection( tools::HHuginn::HClass const*, yaal::dbwrapper::database_ptr_t );
	static tools::HHuginn::value_t query( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t&, int );
	static tools::HHuginn::value_t table_names( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t&, int );
	static tools::HHuginn::value_t column_names( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t&, int );
	static tools::HHuginn::class_t get_class( tools::huginn::HRuntime*, tools::HHuginn::HClass const*, tools::HHuginn::class_t const& );
private:
	tools::HHuginn::value_t do_query( tools::huginn::HThread*, tools::HHuginn::values_t&, int );
	tools::HHuginn::value_t do_table_names( tools::huginn::HThread*, tools::HHuginn::values_t&, int );
	tools::HHuginn::value_t do_column_names( tools::huginn::HThread*, tools::HHuginn::values_t&, int );
private:
	virtual tools::HHuginn::value_t do_clone( tools::huginn::HThread*, tools::HHuginn::value_t*, int ) const override __attribute__((noreturn));
};

}

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HUGINN_DATABASECONNECTION_HXX_INCLUDED */

