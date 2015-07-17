/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  compiledregularexpression.cxx - this file is integral part of `yaal' project.

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
#include "compiledregularexpression.hxx"
#include "regularexpressionmatch.hxx"
#include "helper.hxx"
#include "thread.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HCompiledRegularExpression::HCompiledRegularExpression( HHuginn::HClass const* class_, regex_t regex_ )
	: HHuginn::HObject( class_ )
	, _regex( yaal::move( regex_ ) ) {
	return;
}

HHuginn::value_t HCompiledRegularExpression::groups(
	huginn::HThread* thread_,
	HHuginn::HObject* object_,
	HHuginn::values_t const& values_,
	int position_
) {
	HCompiledRegularExpression* cre( static_cast<HCompiledRegularExpression*>( object_ ) );
	return ( cre->do_groups( thread_, values_, position_ ) );
}

HHuginn::value_t HCompiledRegularExpression::match(
	huginn::HThread* thread_,
	HHuginn::HObject* object_,
	HHuginn::values_t const& values_,
	int position_
) {
	HCompiledRegularExpression* cre( static_cast<HCompiledRegularExpression*>( object_ ) );
	return ( cre->do_match( thread_, values_, position_ ) );
}

class HCompiledRegularExpressionClass : public HHuginn::HClass {
	HHuginn::class_t const& _exceptionClass;
	HHuginn::class_t _regularExpressionMatchClass;
public:
	typedef HCompiledRegularExpressionClass this_type;
	typedef HHuginn::HClass base_type;
	HCompiledRegularExpressionClass(
		HHuginn* huginn_,
		HHuginn::class_t const& exceptionClass_
	) : HHuginn::HClass(
			huginn_,
			HHuginn::HType::register_type( "CompiledRegularExpression", huginn_ ),
			nullptr,
			HHuginn::HClass::field_names_t{
				"match",
				"groups"
			},
			HHuginn::values_t{
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HCompiledRegularExpression::match, _1, _2, _3, _4 ) ),
				make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HCompiledRegularExpression::groups, _1, _2, _3, _4 ) )
			}
		)
		, _exceptionClass( exceptionClass_ )
		, _regularExpressionMatchClass( HRegularExpressionMatch::get_class( huginn_ ) ) {
		return;
	}
	HHuginn::HClass const* remc( void ) const {
		return ( _regularExpressionMatchClass.raw() );
	}
private:
	virtual HHuginn::value_t do_create_instance( huginn::HThread* thread_, HHuginn::values_t const& values_, int position_ ) const {
		M_PROLOG
		verify_arg_count( "CompiledRegularExpression.contructor", values_, 1, 1, position_ );
		HCompiledRegularExpression::regex_t regex( make_resource<HRegex>( get_string( values_[0] ), HRegex::COMPILE::NO_EXCEPTION ) );
		HHuginn::value_t v( _none_ );
		if ( regex->is_valid() ) {
			v = make_pointer<HCompiledRegularExpression>( this, yaal::move( regex ) );
		} else {
			thread_->raise( _exceptionClass.raw(), regex->error() );
		}
		return ( v );
		M_EPILOG
	}
};

HHuginn::value_t HCompiledRegularExpression::do_match(
	huginn::HThread*,
	HHuginn::values_t const& values_,
	int position_
) {
	char const name[] = "CompiledRegularExpression.match";
	verify_arg_count( name, values_, 1, 1, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, true, position_ );
	HCompiledRegularExpressionClass const* creClass( static_cast<HCompiledRegularExpressionClass const*>( HObject::get_class() ) );
	return ( make_pointer<HRegularExpressionMatch>( creClass->remc(), make_resource<HRegex>( _regex->copy() ), values_[0] ) );
}

HHuginn::value_t HCompiledRegularExpression::do_groups(
	huginn::HThread*,
	HHuginn::values_t const& values_,
	int position_
) {
	char const name[] = "CompiledRegularExpression.groups";
	verify_arg_count( name, values_, 1, 1, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, true, position_ );
	yaal::hcore::HString const& string( get_string( values_[0] ) );
	HRegex::groups_t g( _regex->groups( string ) );
	HHuginn::value_t v( _none_ );
	if ( ! g.empty() ) {
		v = make_pointer<HHuginn::HList>();
		HHuginn::HList* l( static_cast<HHuginn::HList*>( v.raw() ) );
		for ( HRegex::HMatch const& m : g ) {
			l->push_back( make_pointer<HHuginn::HString>( string.substr( m.start(), m.size() ) ) );
		}
	}
	return ( v );
}

HHuginn::class_t HCompiledRegularExpression::get_class( HHuginn* huginn_, HHuginn::class_t const& exceptionClass_ ) {
	M_PROLOG
	HHuginn::class_t c(
		make_pointer<HCompiledRegularExpressionClass>(
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

