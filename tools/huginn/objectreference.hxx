/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/objectreference.hxx
 * \brief Declaration of huginn::HObjectReference class.
 */

#ifndef YAAL_TOOLS_HUGINN_OBJECTREFERENCE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_OBJECTREFERENCE_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

/*! \brief Type hierarchy aware object reference.
 *
 * Sole purpose of this class is to handle `super' Huginn keyword.
 */
class HObjectReference : public HValue {
public:
	typedef HObjectReference this_type;
	typedef HValue base_type;
private:
	value_t _value;
	HClass const* _referenceClass;
public:
	HObjectReference( HClass const*, value_t const&, int, int, int );
	HObjectReference( HClass const*, value_t const&, HClass const* );
	int field_index( HHuginn::identifier_id_t const& ) const;
	value_t field( huginn::HThread*, int, int );
	HClass const* reference_class( void ) const {
		return ( _referenceClass );
	}
	HHuginn::value_t const& value( void ) {
		return ( _value );
	}
private:
	HObjectReference( HObjectReference const& ) = delete;
	HObjectReference& operator = ( HObjectReference const& ) = delete;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_OBJECTREFERENCE_HXX_INCLUDED */

