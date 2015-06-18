/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  helper.cxx - this file is integral part of `yaal' project.

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
#include "helper.hxx"
#include "thread.hxx"
#include "keyword.hxx"
#include "tools/util.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

bool is_keyword( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	return ( _keywords_.count( name_ ) > 0 );
	M_EPILOG
}

bool is_builtin( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	return ( ( _keywords_.count( name_ ) > 0 ) || ( _builtin_.count( name_ ) > 0 ) );
	M_EPILOG
}

bool is_restricted( yaal::hcore::HString const& name_ ) {
	M_PROLOG
	return ( ( _keywords_.count( name_ ) > 0 ) || ( _builtin_.count( name_ ) > 0 ) || ( _standardLibrary_.count( name_ ) > 0 ) );
	M_EPILOG
}

void operands_type_mismatch( char const* op_, HHuginn::type_t t1_, HHuginn::type_t t2_, int pos_ ) {
	hcore::HString msg( "Operand types for `" );
	msg.append( op_ )
		.append( "' do not match: " )
		.append( t1_->name() )
		.append( " vs " )
		.append( t2_->name() )
		.append( "." ),
	throw HHuginn::HHuginnRuntimeException( msg, pos_ );
}

void verify_arg_count( yaal::hcore::HString const& name_, HHuginn::values_t const& values_, int min_, int max_, int position_ ) {
	M_PROLOG
	int argCount( static_cast<int>( values_.get_size() ) );
	if ( min_ == max_ ) {
		if ( argCount != min_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"Bad number of parameters in call to: `"_ys
				.append( name_ ).append( "()', expected exactly: " )
				.append( min_ ).append( ", got: " )
				.append( argCount ).append( "." ),
				position_
			);
		}
	} else {
		if ( argCount < min_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"Bad number of parameters in call to: `"_ys
				.append( name_ ).append( "()', expected at least: " )
				.append( min_ ).append( ", got: " )
				.append( argCount ).append( "." ),
				position_
			);
		} else if ( argCount > max_ ) {
			throw HHuginn::HHuginnRuntimeException(
				"Bad number of parameters in call to: `"_ys
				.append( name_ ).append( "()', expected at most: " )
				.append( max_ ).append( ", got: " )
				.append( argCount ).append( "." ),
				position_
			);
		}
	}
	return;
	M_EPILOG
}

void verify_arg_type(
	yaal::hcore::HString const& name_,
	HHuginn::values_t const& values_,
	int no_, HHuginn::type_t type_, bool oneArg_, int position_ ) {
	M_PROLOG
	if ( values_[no_]->type() != type_ ) {
		HString no;
		if ( ! oneArg_ ) {
			no = util::ordinal( no_ + 1 ).append( " " );
		}
		throw HHuginn::HHuginnRuntimeException(
			""_ys.append( name_ )
			.append( "() " )
			.append( no )
			.append( "argument must be a `" )
			.append( type_->name() )
			.append( "', not a `" )
			.append( values_[no_]->type()->name() )
			.append( "'." ),
			position_
		);
	}
	return;
	M_EPILOG
}

yaal::hcore::HString const& get_string( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HString const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HString const*>( value_.raw() )->value() );
}

int long get_integer( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HInteger const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HInteger const*>( value_.raw() )->value() );
}

double long get_real( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HReal const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HReal const*>( value_.raw() )->value() );
}

yaal::hcore::HNumber const& get_number( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HNumber const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HNumber const*>( value_.raw() )->value() );
}

bool get_boolean( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HBoolean const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HBoolean const*>( value_.raw() )->value() );
}

char get_character( HHuginn::value_t const& value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HCharacter const*>( value_.raw() ) );
	return ( static_cast<HHuginn::HCharacter const*>( value_.raw() )->value() );
}

yaal::hcore::HString const& get_string( HHuginn::HObject const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HString const*>( value_ ) );
	return ( static_cast<HHuginn::HString const*>( value_ )->value() );
}

/*
int long get_integer( HHuginn::HObject const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HInteger const*>( value_ ) );
	return ( static_cast<HHuginn::HInteger const*>( value_ )->value() );
}

double long get_real( HHuginn::HObject const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HReal const*>( value_ ) );
	return ( static_cast<HHuginn::HReal const*>( value_ )->value() );
}

yaal::hcore::HNumber const& get_number( HHuginn::HObject const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HNumber const*>( value_ ) );
	return ( static_cast<HHuginn::HNumber const*>( value_ )->value() );
}

bool get_boolean( HHuginn::HObject const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HBoolean const*>( value_ ) );
	return ( static_cast<HHuginn::HBoolean const*>( value_ )->value() );
}

char get_character( HHuginn::HObject const* value_ ) {
	M_ASSERT( !! value_ );
	M_ASSERT( dynamic_cast<HHuginn::HCharacter const*>( value_ ) );
	return ( static_cast<HHuginn::HCharacter const*>( value_ )->value() );
}
*/


}

}

}

