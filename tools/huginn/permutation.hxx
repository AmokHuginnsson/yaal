/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_PERMUTATION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_PERMUTATION_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"
#include "tools/xmath.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HPermutation : public huginn::HIterable {
private:
	HHuginn::values_t _dataSet;
	HHuginn::TYPE _type;
public:
	HPermutation( huginn::HClass const*, HHuginn::values_t&&, HHuginn::TYPE );
	static HHuginn::class_t get_class( HRuntime*, huginn::HClass const* );
protected:
	virtual int long do_size( huginn::HThread*, int ) const override {
		return ( tools::xmath::factorial( _dataSet.get_size() ) );
	}
private:
	virtual iterator_t do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int position_ ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_PERMUTATION_HXX_INCLUDED */

