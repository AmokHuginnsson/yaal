/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_PRODUCT_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_PRODUCT_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HProduct : public huginn::HIterable {
public:
	typedef yaal::hcore::HArray<HHuginn::values_t> axes_t;
private:
	axes_t _axes;
public:
	HProduct( huginn::HClass const* class_, axes_t&& source_ )
		: HIterable( class_ )
		, _axes( yaal::move( source_ ) ) {
	}
	static HHuginn::class_t get_class( HRuntime*, huginn::HClass const* );
protected:
	virtual int long do_size( huginn::HThread*, int ) const override {
		int long s( 1 );
		for ( axes_t::value_type const& a : _axes ) {
			s *= a.get_size();
		}
		return ( s );
	}
private:
	virtual iterator_t do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_PRODUCT_HXX_INCLUDED */

