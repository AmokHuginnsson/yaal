/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "regularexpressionmatch.hxx"
#include "runtime.hxx"
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
		, _objectFactory( _regularExpressionMatch->HValue::get_class()->runtime()->object_factory() ) {
		return;
	}
protected:
	virtual HHuginn::value_t do_value( HThread*, int ) override {
		return ( _objectFactory->create_string( _regularExpressionMatch->get_string().substr( _it->start(), _it->size() ) ) );
	}
	virtual bool do_is_valid( huginn::HThread*, int ) override {
		return ( _it != _regularExpressionMatch->end() );
	}
	virtual void do_next( HThread*, int ) override {
		++ _it;
	}
private:
	HRegularExpressionMatchIterator( HRegularExpressionMatchIterator const& ) = delete;
	HRegularExpressionMatchIterator& operator = ( HRegularExpressionMatchIterator const& ) = delete;
};

HRegularExpressionMatch::HRegularExpressionMatch(
	huginn::HClass const* class_,
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

HHuginn::value_t HRegularExpressionMatch::matched( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "RegularExpressionMatch.matched";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HRegularExpressionMatch* rem( static_cast<HRegularExpressionMatch*>( object_->raw() ) );
	return ( thread_->runtime().boolean_value( static_cast<bool>( rem->_regex->matches( rem->_fast ) ) ) );
	M_EPILOG
}

huginn::HIterable::iterator_t HRegularExpressionMatch::do_iterator( huginn::HThread*, int ) {
	return ( make_pointer<HRegularExpressionMatchIterator>( this, _regex->find( _fast ) ) );
}

int long HRegularExpressionMatch::do_size( huginn::HThread*, int ) const {
	return ( distance( _regex->find( _fast ), _regex->end() ) );
}

HHuginn::class_t HRegularExpressionMatch::get_class( HRuntime* runtime_, huginn::HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"RegularExpressionMatch",
			"The `RegularExpressionMatch` class represents result of regular expression based text search.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "matched", runtime_->create_method( &HRegularExpressionMatch::matched ), "tell if given regular expression pattern matched against given string" }
	};
	c->redefine( nullptr, fd );
	c->set_origin( origin_ );
	runtime_->huginn()->register_class( c );
	return c;
	M_EPILOG
}

HHuginn::value_t HRegularExpressionMatch::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	M_PROLOG
	return ( thread_->object_factory().create<HRegularExpressionMatch>( HIterable::get_class(), make_resource<HRegex>( _regex->copy() ), _string ) );
	M_EPILOG
}

}

}

}

