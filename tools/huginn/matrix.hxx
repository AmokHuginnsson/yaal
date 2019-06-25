/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_MATRIX_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_MATRIX_HXX_INCLUDED 1

#include "hcore/hmatrix.hxx"
#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HMatrix : public HValue {
	typedef yaal::hcore::HMatrix<yaal::hcore::HNumber> arbitrary_precision_matrix_t;
	typedef yaal::hcore::HResource<arbitrary_precision_matrix_t> arbitrary_precision_matrix_ptr_t;
	typedef yaal::hcore::HMatrix<double long> floating_point_matrix_t;
	typedef yaal::hcore::HResource<floating_point_matrix_t> floating_point_matrix_ptr_t;
	typedef yaal::hcore::HVariant<arbitrary_precision_matrix_ptr_t, floating_point_matrix_ptr_t> data_t;
	data_t _data;
public:
	HMatrix( huginn::HThread*, HClass const*, HHuginn::values_t&, int );
	HMatrix( HClass const*, arbitrary_precision_matrix_ptr_t&& );
	HMatrix( HClass const*, floating_point_matrix_ptr_t&& );
	static HHuginn::value_t columns( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t rows( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t get( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t set( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t scale( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t scale_to( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t inverse( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t transpose( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t apply( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t to_string( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime*, HClass const* );
	static HHuginn::value_t create_instance( HClass const*, huginn::HThread*, HHuginn::values_t&, int );
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual void do_operator_add( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual void do_operator_subtract( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual void do_operator_multiply( HThread*, HHuginn::value_t&, HHuginn::value_t const&, int ) override;
	virtual HHuginn::value_t do_operator_modulus( HThread*, HHuginn::value_t const&, int ) const override;
	virtual HHuginn::value_t do_operator_negate( HThread*, HHuginn::value_t const&, int ) const override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_MATRIX_HXX_INCLUDED */

