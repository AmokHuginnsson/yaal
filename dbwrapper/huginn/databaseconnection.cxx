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
	: HHuginn::HObject( class_ )
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
		HHuginn* huginn_,
		HHuginn::type_id_t typeId_,
		HHuginn::class_t const& exceptionClass_
	) : HHuginn::HClass(
			huginn_,
			typeId_,
			huginn_->identifier_id( "DatabaseConnection" ),
			nullptr,
			HHuginn::field_names_t{
				"query",
				"table_names",
				"column_names"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HDatabaseConnection::query, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HDatabaseConnection::table_names, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HDatabaseConnection::column_names, _1, _2, _3, _4 ) )
			}
		)
		, _exceptionClass( exceptionClass_ )
		, _queryClass( huginn::HQuery::get_class( huginn_, exceptionClass_ ) ) {
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
		char const n[] = "DatabaseConnection.contructor";
		verify_arg_count( n, values_, 1, 1, position_ );
		verify_arg_type( n, values_, 0, HHuginn::TYPE::STRING, true, position_ );
		HHuginn::value_t v( thread_->huginn().none_value() );
		try {
			dbwrapper::database_ptr_t db( dbwrapper::util::connect( get_string( values_[0] ) ) );
			v = make_pointer<HDatabaseConnection>( this, db );
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
	char const name[] = "DatabaseConnection.query";
	verify_arg_count( name, values_, 1, 1, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, true, position_ );
	HDatabaseConnectionClass const* dbcClass( static_cast<HDatabaseConnectionClass const*>( HObject::get_class() ) );
	HHuginn::value_t v( thread_->huginn().none_value() );
	try {
		dbwrapper::HQuery::ptr_t q( _database->prepare_query( get_string( values_[0] ) ) );
		v = make_pointer<huginn::HQuery>( dbcClass->query_class(), q );
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
	verify_arg_count( name, values_, 0, 0, position_ );
	HDatabaseConnectionClass const* dbcClass( static_cast<HDatabaseConnectionClass const*>( HObject::get_class() ) );
	HHuginn::value_t v( thread_->huginn().none_value() );
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
	char const name[] = "DatabaseConnection.table_names";
	verify_arg_count( name, values_, 1, 1, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, true, position_ );
	HDatabaseConnectionClass const* dbcClass( static_cast<HDatabaseConnectionClass const*>( HObject::get_class() ) );
	HHuginn::value_t v( thread_->huginn().none_value() );
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

HHuginn::class_t HDatabaseConnection::get_class( HHuginn* huginn_, HHuginn::class_t const& exceptionClass_ ) {
	M_PROLOG
	HHuginn::class_t c(
		huginn_->create_class(
			HHuginn::class_constructor_t(
				[&huginn_, &exceptionClass_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HDatabaseConnectionClass>(
							huginn_,
							typeId_,
							exceptionClass_
						)
					);
				}
			)
		)
	);
	huginn_->register_class( c );
	return ( c );
	M_EPILOG
}

}

}

}

