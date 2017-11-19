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
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HCompiledRegularExpression::HCompiledRegularExpression( HHuginn::HClass const* class_, regex_t regex_ )
	: HHuginn::HValue( class_ )
	, _regex( yaal::move( regex_ ) ) {
	return;
}

HHuginn::value_t HCompiledRegularExpression::groups(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) {
	HCompiledRegularExpression* cre( static_cast<HCompiledRegularExpression*>( object_->raw() ) );
	return ( cre->do_groups( thread_, values_, position_ ) );
}

HHuginn::value_t HCompiledRegularExpression::match(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) {
	HCompiledRegularExpression* cre( static_cast<HCompiledRegularExpression*>( object_->raw() ) );
	return ( cre->do_match( thread_, values_, position_ ) );
}

HHuginn::value_t HCompiledRegularExpression::replace(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) {
	HCompiledRegularExpression* cre( static_cast<HCompiledRegularExpression*>( object_->raw() ) );
	return ( cre->do_replace( thread_, values_, position_ ) );
}

class HCompiledRegularExpressionClass : public HHuginn::HClass {
	HHuginn::class_t const& _exceptionClass;
	HHuginn::class_t _regularExpressionMatchClass;
public:
	typedef HCompiledRegularExpressionClass this_type;
	typedef HHuginn::HClass base_type;
	HCompiledRegularExpressionClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		HHuginn::class_t const& exceptionClass_
	) : HHuginn::HClass(
			runtime_,
			typeId_,
			runtime_->identifier_id( "CompiledRegularExpression" ),
			nullptr,
			HHuginn::field_definitions_t{
				{ "match",   runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HCompiledRegularExpression::match, _1, _2, _3, _4 ) ),   "( *text* ) - find a match of this compiled regular expression in given *text*" },
				{ "groups",  runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HCompiledRegularExpression::groups, _1, _2, _3, _4 ) ),  "( *text* ) - get all matching regular expression groups from this regular expression in given *text*" },
				{ "replace", runtime_->object_factory()->create<HHuginn::HClass::HMethod>( hcore::call( &HCompiledRegularExpression::replace, _1, _2, _3, _4 ) ), "( *text*, *replacement* ) - replace each occurrence of matched groups in *text* with *replacement* pattern" }
			},
			"The `CompiledRegularExpression` class gives access to result of regular expression based text searches."
		)
		, _exceptionClass( exceptionClass_ )
		, _regularExpressionMatchClass( HRegularExpressionMatch::get_class( runtime_ ) ) {
		return;
	}
	HHuginn::HClass const* regular_expression_match_class( void ) const {
		return ( _regularExpressionMatchClass.raw() );
	}
	HHuginn::HClass const* exception_class( void ) const {
		return ( _exceptionClass.raw() );
	}
private:
	virtual HHuginn::value_t do_create_instance( huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) const {
		M_PROLOG
		char const n[] = "CompiledRegularExpression.constructor";
		verify_arg_count( n, values_, 1, 1, thread_, position_ );
		verify_arg_type( n, values_, 0, HHuginn::TYPE::STRING, ARITY::UNARY, thread_, position_ );
		HCompiledRegularExpression::regex_t regex(
			make_resource<HRegex>(
				get_string( values_[0] ),
				HRegex::COMPILE::NO_EXCEPTION
			)
		);
		HHuginn::value_t v( thread_->runtime().none_value() );
		if ( regex->is_valid() ) {
			v = thread_->object_factory().create<HCompiledRegularExpression>( this, yaal::move( regex ) );
		} else {
			thread_->raise( _exceptionClass.raw(), regex->error(), position_ );
		}
		return ( v );
		M_EPILOG
	}
};

HHuginn::value_t HCompiledRegularExpression::do_match(
	huginn::HThread* thread_,
	HHuginn::values_t& values_,
	int position_
) {
	verify_signature( "CompiledRegularExpression.match", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
	HCompiledRegularExpressionClass const* creClass( static_cast<HCompiledRegularExpressionClass const*>( HValue::get_class() ) );
	return ( thread_->object_factory().create<HRegularExpressionMatch>( creClass->regular_expression_match_class(), make_resource<HRegex>( _regex->copy() ), values_[0] ) );
}

HHuginn::value_t HCompiledRegularExpression::do_replace(
	huginn::HThread* thread_,
	HHuginn::values_t& values_,
	int position_
) {
	verify_signature( "CompiledRegularExpression.replace", values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
	HHuginn::value_t v;
	try {
		v = thread_->object_factory().create_string( _regex->replace( get_string( values_[0] ), get_string( values_[1] ) ) );
	} catch ( HException const& e ) {
		HCompiledRegularExpressionClass const* creClass( static_cast<HCompiledRegularExpressionClass const*>( HValue::get_class() ) );
		thread_->raise( creClass->exception_class(), e.what(), position_ );
	}
	return ( v );
}

HHuginn::value_t HCompiledRegularExpression::do_groups(
	huginn::HThread* thread_,
	HHuginn::values_t& values_,
	int position_
) {
	verify_signature( "CompiledRegularExpression.groups", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
	yaal::hcore::HString const& string( get_string( values_[0] ) );
	HRegex::groups_t g( _regex->groups( string ) );
	HRuntime& rt( thread_->runtime() );
	HHuginn::value_t v( rt.none_value() );
	if ( ! g.empty() ) {
		v = thread_->object_factory().create_list();
		HHuginn::HList* l( static_cast<HHuginn::HList*>( v.raw() ) );
		for ( HRegex::HMatch const& m : g ) {
			l->push_back( rt.object_factory()->create_string( string.substr( m.start(), m.size() ) ) );
		}
	}
	return ( v );
}

HHuginn::class_t HCompiledRegularExpression::get_class( HRuntime* runtime_, HHuginn::class_t const& exceptionClass_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_, &exceptionClass_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HCompiledRegularExpressionClass>(
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

HHuginn::value_t HCompiledRegularExpression::do_clone( huginn::HThread* thread_, int ) const {
	M_PROLOG
	return ( thread_->object_factory().create<HCompiledRegularExpression>( HValue::get_class(), make_resource<HRegex>( _regex->copy() ) ) );
	M_EPILOG
}

}

}

}

