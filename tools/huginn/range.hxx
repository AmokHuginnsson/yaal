/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_RANGE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_RANGE_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HRange : public huginn::HIterable {
	int long long _from;
	int long long _stop;
	int long long _step;
public:
	HRange( huginn::HClass const* class_, int long long from_, int long long stop_, int long long step_ )
		: HIterable( class_ )
		, _from( from_ )
		, _stop( stop_ )
		, _step( step_ ) {
	}
	int long long step( void ) const {
		return ( _step );
	}
	int long long stop( void ) const {
		return ( _stop );
	}
	static HHuginn::class_t get_class( HRuntime*, huginn::HClass const* );
protected:
	virtual int long do_size( huginn::HThread*, int ) const override {
		return ( safe_int::cast<int long>( ( _stop + _step - ( _from + 1 ) ) / _step ) );
	}
	static HHuginn::value_t contains( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
private:
	virtual iterator_t do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_RANGE_HXX_INCLUDED */

