/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/huginn/enumeration.hxx
 * \brief Declaration of HEnumerationClass class.
 */

#ifndef YAAL_TOOLS_HUGINN_ENUMERATION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_ENUMERATION_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/runtime.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace enumeration {

class HDesctiption {
public:
	static int const UNDEFINED = -1;
private:
	yaal::hcore::HString _name;
	yaal::hcore::HString _doc;
	int _value;
public:
	HDesctiption( yaal::hcore::HString const& name_, yaal::hcore::HString const& doc_, int value_ = UNDEFINED )
		: _name( name_ )
		, _doc( doc_ )
		, _value( value_ ) {
	}
	yaal::hcore::HString const& name( void ) const {
		return ( _name );
	}
	yaal::hcore::HString const& doc( void ) const {
		return ( _doc );
	}
	int value( void ) const {
		return ( _value );
	}
};

typedef yaal::hcore::HArray<HDesctiption> descriptions_t;

class HEnumerationClass : public HHuginn::HClass {
public:
	typedef HEnumerationClass this_type;
	typedef HHuginn::HClass base_type;
	typedef yaal::hcore::HPointer<HEnumerationClass> ptr_t;
private:
	class HEnumeralClass;
	HHuginn::class_t _valueClass;
public:
	HEnumerationClass(
		HRuntime*,
		HHuginn::type_id_t,
		HHuginn::identifier_id_t,
		descriptions_t const&,
		yaal::hcore::HString const&,
		HHuginn::HClass const*,
		HHuginn::VISIBILITY visibility_
	);
	static HHuginn::value_t to_string( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	HHuginn::HClass const* enumeral_class( void ) const {
		return ( _valueClass.raw() );
	}
	HHuginn::value_t enumeral( HHuginn::HEnumeral::value_type ) const;
private:
	virtual HHuginn::value_t do_create_instance( huginn::HThread*, HHuginn::values_t&, int ) const __attribute__((noreturn));
};

class HEnumeration : HHuginn::HValue {
public:
	HEnumeration( HHuginn::HClass const* );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const __attribute__((noreturn));
};

HHuginn::class_t create_class(
	HRuntime*,
	yaal::hcore::HString const&,
	descriptions_t const&,
	yaal::hcore::HString const&,
	HHuginn::VISIBILITY,
	HHuginn::HClass const* = nullptr
);

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_ENUMERATION_HXX_INCLUDED */

