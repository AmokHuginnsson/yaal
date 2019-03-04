/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_FILTER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_FILTER_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HFilter : public huginn::HIterable {
	HHuginn::value_t _source;
	HHuginn::function_t _function;
	HHuginn::value_t _method;
public:
	HFilter( huginn::HClass const* class_, HHuginn::value_t source_, HHuginn::function_t function_, HHuginn::value_t method_ )
		: HIterable( class_ )
		, _source( source_ )
		, _function( function_ )
		, _method( method_ ) {
	}
	static HHuginn::class_t get_class( HRuntime*, huginn::HClass const* );
protected:
	virtual int long do_size( huginn::HThread*, int ) const override __attribute__(( noreturn ));
private:
	virtual iterator_t do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FILTER_HXX_INCLUDED */

