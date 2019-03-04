/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_ENUMERATOR_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_ENUMERATOR_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"
#include "tools/huginn/integer.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HEnumerator : public huginn::HIterable {
	HHuginn::value_t _source;
	huginn::HInteger::value_type _start;
public:
	HEnumerator( huginn::HClass const* class_, HHuginn::value_t source_, huginn::HInteger::value_type start_ )
		: HIterable( class_ )
		, _source( source_ )
		, _start( start_ ) {
	}
	static HHuginn::class_t get_class( HRuntime*, huginn::HClass const* );
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		return ( static_cast<huginn::HIterable const*>( _source.raw() )->size( thread_, position_ ) );
	}
private:
	virtual iterator_t do_iterator( HThread*, int ) override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ENUMERATOR_HXX_INCLUDED */

