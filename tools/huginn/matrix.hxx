/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_MATRIX_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_MATRIX_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "hcore/hmatrix.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HMatrix : public HHuginn::HValue {
	typedef yaal::hcore::HMatrix<yaal::hcore::HNumber> arbitrary_precision_matrix_t;
	typedef yaal::hcore::HResource<arbitrary_precision_matrix_t> arbitrary_precision_matrix_ptr_t;
	typedef yaal::hcore::HMatrix<double long> floating_point_matrix_t;
	typedef yaal::hcore::HResource<floating_point_matrix_t> floating_point_matrix_ptr_t;
	typedef yaal::hcore::HVariant<arbitrary_precision_matrix_ptr_t, floating_point_matrix_ptr_t> data_t;
	data_t _data;
public:
	HMatrix( huginn::HThread*, HHuginn::HClass const*, HHuginn::values_t&, int );
	HMatrix( HHuginn::HClass const*, arbitrary_precision_matrix_ptr_t&& );
	HMatrix( HHuginn::HClass const*, floating_point_matrix_ptr_t&& );
	static HHuginn::value_t columns( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t rows( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t get( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t set( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t add( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t subtract( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t multiply( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t det( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t scale( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t scale_to( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t inverse( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t transpose( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t apply( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t negate( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t to_string( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime*, HHuginn::HClass const* );
	static HHuginn::value_t create_instance( HHuginn::HClass const*, huginn::HThread*, HHuginn::values_t&, int );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_MATRIX_HXX_INCLUDED */

