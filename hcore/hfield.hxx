/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hfield.hxx
 * \brief Declaration and implementation of HField<> class template.
 */

#ifndef YAAL_HCORE_HFIELD_HXX_INCLUDED
#define YAAL_HCORE_HFIELD_HXX_INCLUDED 1

#include "hcore/algorithm_low.hxx"

namespace yaal {

namespace hcore {

/*! \brief Disguise base class to look like a field.
 *
 * Allows owner class to initialize any of its base classes with a reference to one of its conceptual fields.
 */
template<typename field_t, int const id = 0>
class HField {
protected:
	field_t _field;
public:
	template<typename... arg_t>
	HField( arg_t&&... arg_ )
		: _field( yaal::forward<arg_t>( arg_ )... ) {
		return;
	}
	virtual ~HField( void ) {}
	field_t& ref( void )
		{ return ( _field ); }
	field_t const& ref( void ) const
		{ return ( _field ); }
};

}

}

#endif /* #ifndef YAAL_HCORE_HFIELD_HXX_INCLUDED */

