/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_DBWRAPPER_HUGINN_QUERYRESULT_HXX_INCLUDED
#define YAAL_DBWRAPPER_HUGINN_QUERYRESULT_HXX_INCLUDED 1

#include "dbwrapper/hrecordset.hxx"
#include "tools/huginn/iterable.hxx"

namespace yaal {

namespace dbwrapper {

namespace huginn {

class HQueryResult : public tools::huginn::HIterable {
	dbwrapper::HRecordSet::ptr_t _recordSet;
	dbwrapper::HRecordSet::HIterator _it;
	tools::huginn::HRuntime* _runtime;
public:
	HQueryResult( tools::huginn::HClass const*, dbwrapper::HRecordSet::ptr_t const& );
	static tools::HHuginn::value_t column_name( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t&, int );
	static tools::HHuginn::value_t field_count( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t&, int );
	static tools::HHuginn::value_t insert_id( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t&, int );
	static tools::HHuginn::value_t has_next( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t&, int );
	static tools::HHuginn::value_t fetch_row( tools::huginn::HThread*, tools::HHuginn::value_t*, tools::HHuginn::values_t&, int );
	static tools::HHuginn::class_t get_class( tools::huginn::HRuntime*, tools::huginn::HClass const*, tools::HHuginn::class_t const& );
private:
	virtual iterator_t do_iterator( tools::huginn::HThread*, int ) override;
	virtual int long do_size( yaal::tools::huginn::HThread*, int ) const override;
	virtual tools::HHuginn::value_t do_clone( tools::huginn::HThread*, tools::HHuginn::value_t*, int ) const override __attribute__((noreturn));
	HQueryResult( HQueryResult const& ) = delete;
	HQueryResult& operator = ( HQueryResult const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_DBWRAPPER_HUGINN_QUERYRESULT_HXX_INCLUDED */

