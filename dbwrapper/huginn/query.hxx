/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_DBWRAPPER_HUGINN_QUERY_HXX_INCLUDED
#define YAAL_DBWRAPPER_HUGINN_QUERY_HXX_INCLUDED 1

#include "dbwrapper/hquery.hxx"
#include "tools/huginn/value.hxx"

namespace yaal {

namespace dbwrapper {

namespace huginn {

class HQuery : public tools::huginn::HValue {
	dbwrapper::HQuery::ptr_t _query;
public:
	HQuery( tools::huginn::HClass const*, dbwrapper::HQuery::ptr_t const& );
	static tools::HHuginn::value_t bind( tools::huginn::HThread*, tools::HHuginn::value_t* object_, tools::HHuginn::values_t&, int );
	static tools::HHuginn::value_t execute( tools::huginn::HThread*, tools::HHuginn::value_t* object_, tools::HHuginn::values_t&, int );
	static tools::HHuginn::class_t get_class( tools::huginn::HRuntime*, tools::huginn::HClass const*, tools::HHuginn::class_t const& );
private:
	virtual tools::HHuginn::value_t do_clone( tools::huginn::HThread*, tools::HHuginn::value_t*, int ) const override __attribute__((noreturn));
};

}

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HUGINN_QUERY_HXX_INCLUDED */

