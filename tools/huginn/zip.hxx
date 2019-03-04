/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_ZIP_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_ZIP_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/iterator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HZip : public huginn::HIterable {
	HHuginn::values_t _source;
public:
	HZip( huginn::HClass const* class_, HHuginn::values_t const& source_ )
		: HIterable( class_ )
		, _source( source_ ) {
	}
	HZip( huginn::HClass const* class_, HHuginn::values_t&& source_ )
		: HIterable( class_ )
		, _source( yaal::move( source_ ) ) {
	}
	static HHuginn::class_t get_class( HRuntime*, huginn::HClass const* );
protected:
	virtual int long do_size( huginn::HThread* thread_, int position_ ) const override {
		int long s( meta::max_signed<int long>::value );
		for ( HHuginn::value_t const& v : _source ) {
			int long ss( static_cast<HIterable const*>( v.raw() )->size( thread_, position_ ) );
			if ( ss < s ) {
				s = ss;
			}
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

#endif /* #ifndef YAAL_TOOLS_HUGINN_ZIP_HXX_INCLUDED */

