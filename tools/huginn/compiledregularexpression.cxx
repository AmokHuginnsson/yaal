/* Read yaal/LICENSE.md file for copyright and licensing information. */

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

HCompiledRegularExpression::HCompiledRegularExpression( huginn::HClass const* class_, regex_t regex_ )
	: huginn::HValue( class_ )
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

HHuginn::value_t HCompiledRegularExpression::map(
	huginn::HThread* thread_,
	HHuginn::value_t* object_,
	HHuginn::values_t& values_,
	int position_
) {
	HCompiledRegularExpression* cre( static_cast<HCompiledRegularExpression*>( object_->raw() ) );
	return ( cre->do_map( thread_, values_, position_ ) );
}

class HCompiledRegularExpressionClass : public huginn::HClass {
	HClass const* _exceptionClass;
	HHuginn::class_t _regularExpressionMatchClass;
public:
	typedef HCompiledRegularExpressionClass this_type;
	typedef huginn::HClass base_type;
	HCompiledRegularExpressionClass(
		HRuntime* runtime_,
		HHuginn::type_id_t typeId_,
		huginn::HClass* origin_,
		HClass const* exceptionClass_
	) : huginn::HClass(
			runtime_,
			typeId_,
			runtime_->identifier_id( "CompiledRegularExpression" ),
			"The `CompiledRegularExpression` class gives access to result of regular expression based text searches.",
			HHuginn::ACCESS::PRIVATE
		)
		, _exceptionClass( exceptionClass_ )
		, _regularExpressionMatchClass(
			add_class_as_type_reference(
				origin_,
				HRegularExpressionMatch::get_class( runtime_, origin_ )
			)
		) {
		HHuginn::field_definitions_t fd{
			{ "match",   runtime_->create_method( &HCompiledRegularExpression::match ),   "( *text* ) - find a match of this compiled regular expression in given *text*" },
			{ "groups",  runtime_->create_method( &HCompiledRegularExpression::groups ),  "( *text* ) - get all matching regular expression groups from this regular expression in given *text*" },
			{ "replace", runtime_->create_method( &HCompiledRegularExpression::replace ), "( *text*, *replacement* ) - replace each occurrence of matched groups in *text* with *replacement* pattern" },
			{ "map",     runtime_->create_method( &HCompiledRegularExpression::map ),     "( *text*, *replacer* ) - replace each occurrence of matched groups in *text* by result of calling *map* with value of matched group as an argument" }
		};
		set_origin( origin_ );
		redefine( nullptr, fd );
		return;
	}
	huginn::HClass const* regular_expression_match_class( void ) const {
		return ( _regularExpressionMatchClass.raw() );
	}
	huginn::HClass const* exception_class( void ) const {
		return ( _exceptionClass );
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
			thread_->raise( _exceptionClass, regex->error(), position_ );
		}
		return ( v );
		M_EPILOG
	}
private:
	HCompiledRegularExpressionClass( HCompiledRegularExpressionClass const& ) = delete;
	HCompiledRegularExpressionClass& operator = ( HCompiledRegularExpressionClass const& ) = delete;
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
	HObjectFactory& of( thread_->object_factory() );
	HHuginn::value_t v( of.none_value() );
	try {
		v = of.create_string( _regex->replace( get_string( values_[0] ), get_string( values_[1] ) ) );
	} catch ( hcore::HException const& e ) {
		HCompiledRegularExpressionClass const* creClass( static_cast<HCompiledRegularExpressionClass const*>( HValue::get_class() ) );
		thread_->raise( creClass->exception_class(), e.what(), position_ );
	}
	return ( v );
}

namespace {

yaal::hcore::HString replacer( huginn::HThread* thread_, HHuginn::value_t& replacer_, yaal::hcore::HString const& s_, int position_ ) {
	HObjectFactory& of( thread_->object_factory() );
	HHuginn::value_t a( of.create_string( s_ ) );
	HHuginn::value_t v( replacer_->operator_call( thread_, replacer_, HArguments( thread_, a ), position_ ) );
	if ( v->type_id() != HHuginn::TYPE::STRING ) {
		throw HHuginn::HHuginnRuntimeException(
			"User supplied `replacer` function must return a string type instead of "_ys
				.append( a_type_name( v->get_class() ) )
				.append( "." ),
			thread_->current_frame()->file_id(),
			position_
		);
	}
	return ( get_string( v ) );
}

}

HHuginn::value_t HCompiledRegularExpression::do_map(
	huginn::HThread* thread_,
	HHuginn::values_t& values_,
	int position_
) {
	char const name[] = "CompiledRegularExpression.map";
	verify_arg_count( name, values_, 2, 2, thread_, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, ARITY::MULTIPLE, thread_, position_ );
	verify_arg_callable( name, values_, 1, ARITY::MULTIPLE, thread_, position_ );
	HObjectFactory& of( thread_->object_factory() );
	HHuginn::value_t v( of.none_value() );
	try {
		hcore::HString s( _regex->replace( get_string( values_[0] ), call( replacer, thread_, ref( values_[1] ), _1, position_ ) ) );
		v = of.create_string( yaal::move( s ) );
	} catch ( hcore::HException const& e ) {
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
		huginn::HList* l( static_cast<huginn::HList*>( v.raw() ) );
		for ( HRegex::HMatch const& m : g ) {
			l->push_back( rt.object_factory()->create_string( string.substr( m.start(), m.size() ) ) );
		}
	}
	return ( v );
}

HHuginn::class_t HCompiledRegularExpression::get_class( HRuntime* runtime_, huginn::HClass* origin_, HClass const* exceptionClass_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			HRuntime::class_constructor_t(
				[&runtime_, &origin_, &exceptionClass_] ( HHuginn::type_id_t typeId_ ) -> HHuginn::class_t {
					return (
						make_pointer<HCompiledRegularExpressionClass>(
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

HHuginn::value_t HCompiledRegularExpression::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	M_PROLOG
	return ( thread_->object_factory().create<HCompiledRegularExpression>( HValue::get_class(), make_resource<HRegex>( _regex->copy() ) ) );
	M_EPILOG
}

}

}

}

