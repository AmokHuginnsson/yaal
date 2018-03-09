/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "query.hxx"
#include "queryresult.hxx"
#include "tools/huginn/runtime.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/huginn/helper.hxx"
#include "tools/huginn/objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;
using namespace yaal::dbwrapper;

namespace yaal {

namespace dbwrapper {

namespace huginn {

HQuery::HQuery(
	HHuginn::HClass const* class_,
	dbwrapper::HQuery::ptr_t const& query_
) : HValue( class_ )
	, _query( query_ ) {
	return;
}

class HQueryClass : public HHuginn::HClass {
	HHuginn::class_t const& _exceptionClass;
	HHuginn::class_t _queryResultClass;
public:
	typedef HQueryClass this_type;
	typedef HHuginn::HClass base_type;
	HQueryClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		HHuginn::class_t const& exceptionClass_
	) : HHuginn::HClass(
			runtime_,
			typeId_,
			runtime_->identifier_id( "Query" ),
			nullptr,
			"The `Query` class represents compiled database query. It is used for actual query execution."
		)
		, _exceptionClass( exceptionClass_ )
		, _queryResultClass( huginn::HQueryResult::get_class( runtime_, exceptionClass_ ) ) {
		HHuginn::field_definitions_t fd{
			{ "bind",    runtime_->create_method( &HQuery::bind ),    "( *index*, *value* ) - bind given *value* for query variable at given *index*" },
			{ "execute", runtime_->create_method( &HQuery::execute ), "execute query" }
		};
		redefine( nullptr, fd );
		return;
	}
	HHuginn::HClass const* query_result_class( void ) const {
		return ( _queryResultClass.raw() );
	}
	HHuginn::HClass const* exception_class( void ) const {
		return ( _exceptionClass.raw() );
	}
};

HHuginn::value_t HQuery::bind( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Query.bind", values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::STRING }, thread_, position_ );
	HQuery* q( static_cast<HQuery*>( object_->raw() ) );
	HQueryClass const* qc( static_cast<HQueryClass const*>( q->HValue::get_class() ) );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		q->_query->bind( static_cast<int>( get_integer( values_[0] ) ), get_string( values_[1] ) );
		v = *object_;
	} catch ( HException const& e ) {
		thread_->raise( qc->exception_class(), e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HQuery::execute( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Query.execute";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HQuery* q( static_cast<HQuery*>( object_->raw() ) );
	HQueryClass const* qc( static_cast<HQueryClass const*>( q->HValue::get_class() ) );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		HRecordSet::ptr_t rs( q->_query->execute() );
		v = thread_->object_factory().create<HQueryResult>( qc->query_result_class(), rs );
	} catch ( HException const& e ) {
		thread_->raise( qc->exception_class(), e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::class_t HQuery::get_class( HRuntime* runtime_, HHuginn::class_t const& exceptionClass_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_, &exceptionClass_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HQueryClass>(
							runtime_,
							typeId_,
							exceptionClass_
						)
					);
				}
			)
		)
	);
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HQuery::do_clone( tools::huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on Query.", thread_->current_frame()->file_id(), position_ );
	M_EPILOG
}

}

}

}

