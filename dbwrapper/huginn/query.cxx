/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  query.cxx - this file is integral part of `yaal' project.

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
			HHuginn::field_definitions_t{
				{ "bind",    runtime_->object_factory()->create_method( hcore::call( &HQuery::bind, _1, _2, _3, _4 ) ),    "( *index*, *value* ) - bind given *value* for query variable at given *index*" },
				{ "execute", runtime_->object_factory()->create_method( hcore::call( &HQuery::execute, _1, _2, _3, _4 ) ), "execute query" }
			},
			"The `Query` class represents compiled database query. It is used for actual query execution."
		)
		, _exceptionClass( exceptionClass_ )
		, _queryResultClass( huginn::HQueryResult::get_class( runtime_, exceptionClass_ ) ) {
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

HHuginn::value_t HQuery::do_clone( tools::huginn::HThread* thread_, int position_ ) const {
	M_PROLOG
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on Query.", thread_->current_frame()->file_id(), position_ );
	M_EPILOG
}

}

}

}

