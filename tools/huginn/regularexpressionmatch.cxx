/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  regularexpressionmatch.cxx - this file is integral part of `yaal' project.

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
#include "regularexpressionmatch.hxx"
#include "iterator.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HRegularExpressionMatchIterator : public HIteratorInterface {
	HRegularExpressionMatch* _regularExpressionMatch;
	HRegex::HMatchIterator _it;
	HObjectFactory* _objectFactory;
public:
	HRegularExpressionMatchIterator(
		HRegularExpressionMatch* regularExpressionMatch_,
		HRegex::HMatchIterator const& it_
	) : _regularExpressionMatch( regularExpressionMatch_ )
		, _it( it_ )
		, _objectFactory( _regularExpressionMatch->HObject::get_class()->huginn()->object_factory() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( void ) override {
		return ( _objectFactory->create_string( _regularExpressionMatch->get_string().substr( _it->start(), _it->size() ) ) );
	}
	virtual bool do_is_valid( void ) override {
		return ( _it != _regularExpressionMatch->end() );
	}
	virtual void do_next( void ) override {
		++ _it;
	}
private:
	HRegularExpressionMatchIterator( HRegularExpressionMatchIterator const& ) = delete;
	HRegularExpressionMatchIterator& operator = ( HRegularExpressionMatchIterator const& ) = delete;
};

HRegularExpressionMatch::HRegularExpressionMatch(
	HHuginn::HClass const* class_,
	HCompiledRegularExpression::regex_t regularExpressionMatch_,
	HHuginn::value_t const& string_
) : HIterable( class_ )
	, _regex( yaal::move( regularExpressionMatch_ ) )
	, _string( string_ )
	, _fast( huginn::get_string( _string ) ) {
	return;
}

yaal::hcore::HString const& HRegularExpressionMatch::get_string( void ) const {
	return ( _fast );
}

yaal::hcore::HRegex::HMatchIterator HRegularExpressionMatch::end( void ) const {
	return ( _regex->end() );
}

HHuginn::value_t HRegularExpressionMatch::matched( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "RegularExpressionMatch.matched";
	verify_arg_count( name, values_, 0, 0, position_ );
	HRegularExpressionMatch* rem( static_cast<HRegularExpressionMatch*>( object_->raw() ) );
	return ( thread_->object_factory().create_boolean( rem->_regex->matches( rem->_fast ) ) );
	M_EPILOG
}

HHuginn::HIterable::HIterator HRegularExpressionMatch::do_iterator( void ) {
	HIterator::iterator_implementation_t impl( new ( memory::yaal ) HRegularExpressionMatchIterator( this, _regex->find( _fast ) ) );
	return ( HIterator( yaal::move( impl ) ) );
}

int long HRegularExpressionMatch::do_size( void ) const {
	return ( distance( _regex->find( _fast ), _regex->end() ) );
}

HHuginn::class_t HRegularExpressionMatch::get_class( HHuginn* huginn_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HHuginn::HClass>(
			huginn_,
			HHuginn::HType::register_type( "RegularExpressionMatch", huginn_ ),
			nullptr,
			HHuginn::HClass::field_names_t{
				"matched"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HRegularExpressionMatch::matched, _1, _2, _3, _4 ) )
			}
		)
	);
	return ( c );
	M_EPILOG
}

}

}

}

