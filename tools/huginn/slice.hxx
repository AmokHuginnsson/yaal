/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_SLICE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_SLICE_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HSlice : public HHuginn::HIterable {
	HHuginn::value_t _source;
	int long long _from;
	int long long _stop;
	int long long _step;
public:
	HSlice( HHuginn::HClass const*, HHuginn::value_t const&, int long long, int long long, int long long );
	int long long step( void ) const {
		return ( _step );
	}
	int long long stop( void ) const {
		return ( _stop );
	}
	static HHuginn::class_t get_class( HRuntime* runtime_, HHuginn::HClass const* origin_ );
protected:
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	virtual iterator_t do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_SLICE_HXX_INCLUDED */

