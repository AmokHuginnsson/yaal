/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  databaseconnection.cxx - this file is integral part of `yaal' project.

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
#include "databaseconnection.hxx"
#include "query.hxx"
#include "dbwrapper/dbwrapper.hxx"
#include "tools/huginn/runtime.hxx"
#include "tools/huginn/helper.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/huginn/objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace dbwrapper {

namespace huginn {

HDatabaseConnection::HDatabaseConnection( HHuginn::HClass const* class_, dbwrapper::database_ptr_t database_ )
	: HHuginn::HValue( class_ )
	, _database( database_ ) {
	return;
}

HHuginn::value_t HDatabaseConnection::query(
	tools::huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t const& values_,
	int position_
) {
	HDatabaseConnection* dbc( static_cast<HDatabaseConnection*>( object_->raw() ) );
	return ( dbc->do_query( thread_, values_, position_ ) );
}

HHuginn::value_t HDatabaseConnection::table_names(
	tools::huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t const& values_,
	int position_
) {
	HDatabaseConnection* dbc( static_cast<HDatabaseConnection*>( object_->raw() ) );
	return ( dbc->do_table_names( thread_, values_, position_ ) );
}

HHuginn::value_t HDatabaseConnection::column_names(
	tools::huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t const& values_,
	int position_
) {
	HDatabaseConnection* dbc( static_cast<HDatabaseConnection*>( object_->raw() ) );
	return ( dbc->do_column_names( thread_, values_, position_ ) );
}

class HDatabaseConnectionClass : public HHuginn::HClass {
	HHuginn::class_t const& _exceptionClass;
	HHuginn::class_t _queryClass;
public:
	typedef HDatabaseConnectionClass this_type;
	typedef HHuginn::HClass base_type;
	HDatabaseConnectionClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		HHuginn::class_t const& exceptionClass_
	) : HHuginn::HClass(
			runtime_,
			typeId_,
			runtime_->identifier_id( "DatabaseConnection" ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "query",        runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HDatabaseConnection::query, _1, _2, _3, _4 ) ),        "( *sql* ) - create query object for given *sql* `string`" },
				{ "table_names",  runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HDatabaseConnection::table_names, _1, _2, _3, _4 ) ),  "get list of table names available in connected database" },
				{ "column_names", runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HDatabaseConnection::column_names, _1, _2, _3, _4 ) ), "( *table* ) - get list of column names from given *table* in connected database" }
			},
			"The `DatabaseConnection` class allows performing various type of queries on connected database."
		)
		, _exceptionClass( exceptionClass_ )
		, _queryClass( huginn::HQuery::get_class( runtime_, exceptionClass_ ) ) {
		return;
	}
	HHuginn::HClass const* query_class( void ) const {
		return ( _queryClass.raw() );
	}
	HHuginn::HClass const* exception_class( void ) const {
		return ( _exceptionClass.raw() );
	}
private:
	virtual HHuginn::value_t do_create_instance( tools::huginn::HThread* thread_, HHuginn::values_t const& values_, int position_ ) const {
		M_PROLOG
		verify_signature( "DatabaseConnection.constructor", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HHuginn::value_t v( thread_->runtime().none_value() );
		try {
			dbwrapper::database_ptr_t db( dbwrapper::util::connect( get_string( values_[0] ) ) );
			v = thread_->object_factory().create<HDatabaseConnection>( this, db );
		} catch ( HException const& e ) {
			thread_->raise( exception_class(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
};

HHuginn::value_t HDatabaseConnection::do_query(
	tools::huginn::HThread* thread_,
	HHuginn::values_t const& values_,
	int position_
) {
	verify_signature( "DatabaseConnection.query", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
	HDatabaseConnectionClass const* dbcClass( static_cast<HDatabaseConnectionClass const*>( HValue::get_class() ) );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		dbwrapper::HQuery::ptr_t q( _database->prepare_query( get_string( values_[0] ) ) );
		v = thread_->object_factory().create<huginn::HQuery>( dbcClass->query_class(), q );
	} catch ( HException const& e ) {
		thread_->raise( dbcClass->exception_class(), e.what(), position_ );
	}
	return ( v );
}

HHuginn::value_t HDatabaseConnection::do_table_names(
	tools::huginn::HThread* thread_,
	HHuginn::values_t const& values_,
	int position_
) {
	char const name[] = "DatabaseConnection.table_names";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HDatabaseConnectionClass const* dbcClass( static_cast<HDatabaseConnectionClass const*>( HValue::get_class() ) );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		HDataBase::table_list_t tl( _database->get_tables() );
		v = thread_->object_factory().create_list();
		HHuginn::HList* l( static_cast<HHuginn::HList*>( v.raw() ) );
		for ( yaal::hcore::HString const& tn : tl ) {
			l->push_back( thread_->object_factory().create_string( tn ) );
		}
	} catch ( HException const& e ) {
		thread_->raise( dbcClass->exception_class(), e.what(), position_ );
	}
	return ( v );
}

HHuginn::value_t HDatabaseConnection::do_column_names(
	tools::huginn::HThread* thread_,
	HHuginn::values_t const& values_,
	int position_
) {
	verify_signature( "DatabaseConnection.column_names", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
	HDatabaseConnectionClass const* dbcClass( static_cast<HDatabaseConnectionClass const*>( HValue::get_class() ) );
	HHuginn::value_t v( thread_->runtime().none_value() );
	try {
		HDataBase::column_list_t cl( _database->get_columns( get_string( values_[0] ) ) );
		v = thread_->object_factory().create_list();
		HHuginn::HList* l( static_cast<HHuginn::HList*>( v.raw() ) );
		for ( yaal::hcore::HString const& cn : cl ) {
			l->push_back( thread_->object_factory().create_string( cn ) );
		}
	} catch ( HException const& e ) {
		thread_->raise( dbcClass->exception_class(), e.what(), position_ );
	}
	return ( v );
}

HHuginn::class_t HDatabaseConnection::get_class( HRuntime* runtime_, HHuginn::class_t const& exceptionClass_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_, &exceptionClass_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HDatabaseConnectionClass>(
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

HHuginn::value_t HDatabaseConnection::do_clone( tools::huginn::HThread*, int ) const {
	M_PROLOG
	throw HHuginn::HHuginnRuntimeException( "Copy semantics is not supported on DatabaseConnection.", 0 );
	M_EPILOG
}

}

}

}

