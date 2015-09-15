/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  exception.cxx - this file is integral part of `yaal' project.

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
#include "exception.hxx"
#include "tools/hhuginn.hxx"
#include "iterator.hxx"
#include "helper.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace exception {

class HExceptionClass : public HHuginn::HClass {
public:
	typedef HExceptionClass this_type;
	typedef HHuginn::HClass base_type;
	HExceptionClass(
		HHuginn* huginn_,
		yaal::hcore::HString const& name_,
		HHuginn::HClass const* base_
	) : HHuginn::HClass(
			huginn_,
			HHuginn::HType::register_type( name_, huginn_ ),
			base_,
			base_
				? HHuginn::HClass::field_names_t{}
				: HHuginn::HClass::field_names_t{
					"what",
					"where",
					"message"
				},
			base_
				? HHuginn::values_t{}
				: HHuginn::values_t{
					make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HExceptionClass::what, _1, _2, _3, _4 ) ),
					make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HExceptionClass::where, _1, _2, _3, _4 ) ),
					make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HExceptionClass::message, _1, _2, _3, _4 ) )
				}
		) {
		return;
	}
private:
	virtual HHuginn::value_t do_create_instance( huginn::HThread*, HHuginn::values_t const& values_, int position_ ) const {
		M_PROLOG
		HString n( name() + ".contructor" );
		verify_arg_count( n, values_, 1, 1, position_ );
		verify_arg_type( n, values_, 0, HHuginn::TYPE::STRING, true, position_ );
		return ( make_pointer<HHuginn::HException>( this, get_string( values_[0] ) ) );
		M_EPILOG
	}
	static HHuginn::value_t what( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		HHuginn::HException* e( static_cast<HHuginn::HException*>( object_ ) );
		verify_arg_count(
			static_cast<HExceptionClass const*>( e->get_class() )->name() + ".what",
			values_, 0, 0, position_
		);
		return ( make_pointer<HHuginn::HString>( e->what() ) );
		M_EPILOG
	}
	static HHuginn::value_t where( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		HHuginn::HException* e( static_cast<HHuginn::HException*>( object_ ) );
		verify_arg_count(
			static_cast<HExceptionClass const*>( e->get_class() )->name() + ".where",
			values_, 0, 0, position_
		);
		return ( make_pointer<HHuginn::HString>( e->where() ) );
		M_EPILOG
	}
	static HHuginn::value_t message( huginn::HThread*, HHuginn::HObject* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		HHuginn::HException* e( static_cast<HHuginn::HException*>( object_ ) );
		verify_arg_count(
			static_cast<HExceptionClass const*>( e->get_class() )->name() + ".message",
			values_, 0, 0, position_
		);
		HString message( e->where() );
		message.append( ": " ).append( e->what() );
		return ( make_pointer<HHuginn::HString>( message ) );
		M_EPILOG
	}
};

HHuginn::class_t _exceptionClass_( make_pointer<HExceptionClass>( nullptr, "Exception", nullptr ) );

HHuginn::class_t create_class( HHuginn* huginn_, yaal::hcore::HString const& name_, HHuginn::HClass const* base_ ) {
	M_PROLOG
	HHuginn::class_t c( huginn_ ? huginn_->get_class( name_ ) : nullptr );
	if ( ! c ) {
		c = make_pointer<HExceptionClass>(
			huginn_,
			name_,
			base_ ? base_ : _exceptionClass_.raw()
		);
	}
	if ( huginn_ ) {
		huginn_->register_class( c );
	}
	return ( c );
	M_EPILOG
}

HHuginn::class_t _conversionExceptionClass_ = exception::create_class( nullptr, "ConversionException" );
HHuginn::class_t _arithmeticExceptionClass_ = exception::create_class( nullptr, "ArithmeticException" );

}

HHuginn::HClass const* _exception_( exception::_exceptionClass_.raw() );
HHuginn::HClass const* _conversionException_( exception::_conversionExceptionClass_.raw() );
HHuginn::HClass const* _arithmeticException_( exception::_arithmeticExceptionClass_.raw() );

}

HHuginn::HException::HException( HHuginn::HClass const* class_, yaal::hcore::HString const& message_ )
	: HObject( class_ )
	, _message( message_ )
	, _where() {
	return;
}

yaal::hcore::HString const& HHuginn::HException::what( void ) const {
	return ( _message );
}

yaal::hcore::HString const& HHuginn::HException::where( void ) const {
	return ( _where );
}

void HHuginn::HException::set_where( yaal::hcore::HString const& where_ ) {
	_where = where_;
}

HHuginn::value_t HHuginn::HException::do_clone( HHuginn* ) const {
	return ( get_pointer() );
}

}

}

