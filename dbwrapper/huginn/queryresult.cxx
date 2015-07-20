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
#include "tools/huginn/iterator.hxx"
#include "tools/huginn/helper.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace dbwrapper {

namespace huginn {

class HQueryResultIterator : public HIteratorInterface {
	HRecordSet::ptr_t _recordSet;
	HRecordSet::HIterator _it;
public:
	HQueryResultIterator( HRecordSet::ptr_t const& recordSet_, HRecordSet::HIterator&& it_ )
		: _recordSet( recordSet_ )
		, _it( yaal::move( it_ ) ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		HHuginn::value_t v( make_pointer<HHuginn::HList>() );
		HHuginn::HList* row( static_cast<HHuginn::HList*>( v.raw() ) );
		for ( int i( 0 ); i < _recordSet->get_field_count(); ++ i ) {
			HRecordSet::value_t f( _it[i] );
			if ( !! f ) {
				row->push_back( make_pointer<HHuginn::HString>( yaal::move( *f ) ) );
			} else {
				row->push_back( _none_ );
			}
		}
		return ( v );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _recordSet->end() );
	}
	virtual void do_next( void ) override {
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
	, _recordSet( recordSet_ ) {
	return;
}

HHuginn::value_t HQueryResult::column_name( tools::huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "QueryResult.column_name";
	verify_arg_count( name, values_, 1, 1, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, true, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_ ) );
	return ( make_pointer<HHuginn::HString>( qr->_recordSet->get_column_name( static_cast<int>( get_integer( values_[0] ) ) ) ) );
	M_EPILOG
}

HHuginn::value_t HQueryResult::field_count( tools::huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "QueryResult.field_count";
	verify_arg_count( name, values_, 0, 0, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_ ) );
	return ( make_pointer<HHuginn::HInteger>( qr->_recordSet->get_field_count() ) );
	M_EPILOG
}

HHuginn::value_t HQueryResult::insert_id( tools::huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "QueryResult.insert_id";
	verify_arg_count( name, values_, 0, 0, position_ );
	HQueryResult* qr( static_cast<HQueryResult*>( object_ ) );
	return ( make_pointer<HHuginn::HInteger>( qr->_recordSet->get_insert_id() ) );
	M_EPILOG
}

HHuginn::HIterable::HIterator HQueryResult::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HQueryResultIterator( _recordSet, _recordSet->begin() ) );
	return ( HIterator( yaal::move( impl ) ) );
}

class HQueryResultClass : public HHuginn::HClass {
	HHuginn::class_t const& _exceptionClass;
public:
	typedef HQueryResultClass this_type;
	typedef HHuginn::HClass base_type;
	HQueryResultClass(
		HHuginn* huginn_,
		HHuginn::class_t const& exceptionClass_
	) : HHuginn::HClass(
			huginn_,
			HHuginn::HType::register_type( "QueryResult", huginn_ ),
			nullptr,
			HHuginn::HClass::field_names_t{
				"column_name",
				"field_count",
				"insert_id"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HQueryResult::column_name, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HQueryResult::field_count, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HQueryResult::insert_id, _1, _2, _3, _4 ) )
			}
		)
		, _exceptionClass( exceptionClass_ ) {
		return;
	}
	HHuginn::HClass const* exception_class( void ) const {
		return ( _exceptionClass.raw() );
	}
};

HHuginn::class_t HQueryResult::get_class( HHuginn* huginn_, HHuginn::class_t const& exceptionClass_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HQueryResultClass>(
			huginn_,
			exceptionClass_
		)
	);
	return ( c );
	M_EPILOG
}

}

}

}

