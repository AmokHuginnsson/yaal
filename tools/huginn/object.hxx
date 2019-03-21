/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/object.hxx
 * \brief Declaration of huginn::HObject class.
 */

#ifndef YAAL_TOOLS_HUGINN_OBJECT_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_OBJECT_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HObject : public HValue {
public:
	typedef HObject this_type;
	typedef HValue base_type;
	typedef yaal::hcore::HArray<value_t> fields_t;
private:
	fields_t _fields;
public:
	HObject( HClass const*, fields_t const& );
	value_t& field_ref( int );
	HHuginn::value_t call_method( huginn::HThread*, HHuginn::value_t const&, HHuginn::identifier_id_t const&, HHuginn::values_t&, int ) const;
	HHuginn::value_t get_method( huginn::HThread*, HHuginn::value_t const&, HHuginn::identifier_id_t const&, int ) const;
	static HHuginn::value_t init_base( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t forward_call( HHuginn::function_t, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
private:
	virtual void do_destroy( HHuginn::value_t* ) override;
	HHuginn::function_t const& get_method( huginn::HThread*, HHuginn::identifier_id_t const&, int ) const;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual value_t do_field( HHuginn::value_t const&, int ) const override;
	value_t const& do_field( int ) const override;
private:
	HObject( HObject const& ) = delete;
	HObject& operator = ( HObject const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_OBJECT_HXX_INCLUDED */

