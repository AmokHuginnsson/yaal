/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
	tools::huginn::HList* row( static_cast<tools::huginn::HList*>( v.raw() ) );
	HHuginn::value_t const& none( runtime_.none_value() );
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
	HClass const* class_,
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
	HHuginn::values_t& values_,
	int position_
) {
	M_PROLOG
	verify_signature( "QueryResult.column_name", values_, { HHuginn::TYPE::INTEGER }, thread_, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_->raw() ) );
	return ( thread_->object_factory().create_string( qr->_recordSet->get_column_name( static_cast<int>( get_integer( values_[0] ) ) ) ) );
	M_EPILOG
}

HHuginn::value_t HQueryResult::field_count( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "QueryResult.field_count";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_->raw() ) );
	return ( thread_->object_factory().create_integer( qr->_recordSet->get_field_count() ) );
	M_EPILOG
}

HHuginn::value_t HQueryResult::insert_id( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "QueryResult.insert_id";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_->raw() ) );
	return ( thread_->object_factory().create_integer( qr->_recordSet->get_insert_id() ) );
	M_EPILOG
}

HHuginn::value_t HQueryResult::has_next( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "QueryResult.has_next";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_->raw() ) );
	return ( thread_->runtime().boolean_value( qr->_it != qr->_recordSet->end() ) );
	M_EPILOG
}

HIterable::iterator_t HQueryResult::do_iterator( tools::huginn::HThread*, int ) {
	return ( make_pointer<HQueryResultIterator>( _recordSet, yaal::move( _it ), _runtime ) );
}

int long HQueryResult::do_size( tools::huginn::HThread*, int ) const {
	return ( _recordSet->get_dml_size() );
}

class HQueryResultClass : public HClass {
	HHuginn::class_t const& _exceptionClass;
public:
	typedef HQueryResultClass this_type;
	typedef HClass base_type;
	HQueryResultClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		HClass const* origin_,
		HHuginn::class_t const& exceptionClass_
	) : HClass(
			runtime_,
			typeId_,
			runtime_->identifier_id( "QueryResult" ),
			"The `QueryResult` class represent result of database query execution. It allows getting both material query results and query result meta data.",
			HHuginn::ACCESS::PRIVATE
		)
		, _exceptionClass( exceptionClass_ ) {
		HHuginn::field_definitions_t fd{
			{ "column_name", runtime_->create_method( &HQueryResult::column_name ), "( *index* ) - get name of SELECT's column at given *index*" },
			{ "field_count", runtime_->create_method( &HQueryResult::field_count ), "get number of SELECTed attributes" },
			{ "insert_id",   runtime_->create_method( &HQueryResult::insert_id ),   "get value of row ID from last INSERT statement" },
			{ "has_next",    runtime_->create_method( &HQueryResult::has_next ),    "tell if this result set has more rows to be fetched" },
			{ "fetch_row",   runtime_->create_method( &HQueryResult::fetch_row ),   "fetch next row of data from this result set" }
		};
		redefine( nullptr, fd );
		set_origin( origin_ );
		return;
	}
	HClass const* exception_class( void ) const {
		return ( _exceptionClass.raw() );
	}
};

HHuginn::value_t HQueryResult::fetch_row( tools::huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "QueryResult.fetch_row";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_->raw() ) );
	HQueryResultClass const* qrc( static_cast<HQueryResultClass const*>( qr->HValue::get_class() ) );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		v = huginn::fetch_row( qr->_recordSet, qr->_it, thread_->runtime() );
		++ qr->_it;
	} catch ( hcore::HException const& e ) {
		thread_->raise( qrc->exception_class(), e.what(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::class_t HQueryResult::get_class( HRuntime* runtime_, HClass const* origin_, HHuginn::class_t const& exceptionClass_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_, &origin_, &exceptionClass_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HQueryResultClass>(
							runtime_,
							typeId_,
							origin_,
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

HHuginn::value_t HQueryResult::do_clone( tools::huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const {
	M_PROLOG
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on QueryResult.", thread_->current_frame()->file_id(), position_ );
	M_EPILOG
}

}

}

}

