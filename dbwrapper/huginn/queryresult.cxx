/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  queryresult.cxx - this file is integral part of `yaal' project.

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
#include "queryresult.hxx"
#include "tools/huginn/runtime.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/huginn/iterator.hxx"
#include "tools/huginn/helper.hxx"
#include "tools/huginn/objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace dbwrapper {

namespace huginn {

namespace {
HHuginn::value_t fetch_row( HRecordSet::ptr_t const& rs_, HRecordSet::HIterator& it_, HRuntime& runtime_ ) {
	HObjectFactory& objectFactory( *runtime_.object_factory() );
	HHuginn::value_t v( objectFactory.create_list() );
	HHuginn::HList* row( static_cast<HHuginn::HList*>( v.raw() ) );
	HHuginn::value_t& none( runtime_.none_value() );
	for ( int i( 0 ), fieldCount( rs_->get_field_count() ); i < fieldCount; ++ i ) {
		HRecordSet::value_t f( it_[i] );
		if ( !! f ) {
			row->push_back( objectFactory.create_string( yaal::move( *f ) ) );
		} else {
			row->push_back( none );
		}
	}
	return ( v );
}
}

class HQueryResultIterator : public HIteratorInterface {
	HRecordSet::ptr_t _recordSet;
	HRecordSet::HIterator _it;
	HRuntime* _runtime;
public:
	HQueryResultIterator( HRecordSet::ptr_t const& recordSet_, HRecordSet::HIterator&& it_, HRuntime* runtime_ )
		: _recordSet( recordSet_ )
		, _it( yaal::move( it_ ) )
		, _runtime( runtime_ ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( tools::huginn::HThread*, int ) override {
		return ( fetch_row( _recordSet, _it, *_runtime ) );
	}
	virtual bool do_is_valid( tools::huginn::HThread*, int ) override {
		return ( _it != _recordSet->end() );
	}
	virtual void do_next( tools::huginn::HThread*, int ) override {
		++ _it;
	}
private:
	HQueryResultIterator( HQueryResultIterator const& ) = delete;
	HQueryResultIterator& operator = ( HQueryResultIterator const& ) = delete;
};

HQueryResult::HQueryResult(
	HHuginn::HClass const* class_,
	HRecordSet::ptr_t const& recordSet_
) : HIterable( class_ )
	, _recordSet( recordSet_ )
	, _it( _recordSet->begin() )
	, _runtime( class_->runtime() ) {
	return;
}

HHuginn::value_t HQueryResult::column_name(
	tools::huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t const& values_,
	int position_
) {
	M_PROLOG
	verify_signature( "QueryResult.column_name", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_->raw() ) );
	return ( thread_->object_factory().create_string( qr->_recordSet->get_column_name( static_cast<int>( get_integer( values_[0] ) ) ) ) );
	M_EPILOG
}

HHuginn::value_t HQueryResult::field_count( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "QueryResult.field_count";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_->raw() ) );
	return ( thread_->object_factory().create_integer( qr->_recordSet->get_field_count() ) );
	M_EPILOG
}

HHuginn::value_t HQueryResult::insert_id( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "QueryResult.insert_id";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_->raw() ) );
	return ( thread_->object_factory().create_integer( qr->_recordSet->get_insert_id() ) );
	M_EPILOG
}

HHuginn::value_t HQueryResult::has_next( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "QueryResult.has_next";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_->raw() ) );
	return ( thread_->object_factory().create_boolean( qr->_it != qr->_recordSet->end() ) );
	M_EPILOG
}

HHuginn::HIterable::HIterator HQueryResult::do_iterator( tools::huginn::HThread*, int ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HQueryResultIterator( _recordSet, yaal::move( _it ), _runtime ) );
	return ( HIterator( yaal::move( impl ) ) );
}

int long HQueryResult::do_size( tools::huginn::HThread*, int ) const {
	return ( _recordSet->get_dml_size() );
}

class HQueryResultClass : public HHuginn::HClass {
	HHuginn::class_t const& _exceptionClass;
public:
	typedef HQueryResultClass this_type;
	typedef HHuginn::HClass base_type;
	HQueryResultClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		HHuginn::class_t const& exceptionClass_
	) : HHuginn::HClass(
			runtime_,
			typeId_,
			runtime_->identifier_id( "QueryResult" ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "column_name", runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HQueryResult::column_name, _1, _2, _3, _4 ) ), "( *index* ) - get name of SELECT's column at given *index*" },
				{ "field_count", runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HQueryResult::field_count, _1, _2, _3, _4 ) ), "get number of SELECTed attributes" },
				{ "insert_id",   runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HQueryResult::insert_id, _1, _2, _3, _4 ) ),   "get value of row ID from last INSERT statement" },
				{ "has_next",    runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HQueryResult::has_next, _1, _2, _3, _4 ) ),    "tell if this result set has more rows to be fetched" },
				{ "fetch_row",   runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HQueryResult::fetch_row, _1, _2, _3, _4 ) ),   "fetch next row of data from this result set" }
			},
			"The `QueryResult` class represent result of database query execution. It allows getting both material query results and query result meta data."
		)
		, _exceptionClass( exceptionClass_ ) {
		return;
	}
	HHuginn::HClass const* exception_class( void ) const {
		return ( _exceptionClass.raw() );
	}
};

HHuginn::value_t HQueryResult::fetch_row( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "QueryResult.fetch_row";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_->raw() ) );
	HQueryResultClass const* qrc( static_cast<HQueryResultClass const*>( qr->HValue::get_class() ) );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = huginn::fetch_row( qr->_recordSet, qr->_it, thread_->runtime() );
		++ qr->_it;
	} catch ( HException const& e ) {
		thread_->raise( qrc->exception_class(), e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::class_t HQueryResult::get_class( HRuntime* runtime_, HHuginn::class_t const& exceptionClass_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_, &exceptionClass_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HQueryResultClass>(
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

HHuginn::value_t HQueryResult::do_clone( tools::huginn::HThread* thread_, int position_ ) const {
	M_PROLOG
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on QueryResult.", thread_->current_frame()->file_id(), position_ );
	M_EPILOG
}

}

}

}

