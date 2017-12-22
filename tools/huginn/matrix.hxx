/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  matrix.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

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
	static HHuginn::value_t to_string( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime* );
	static HHuginn::value_t create_instance( HHuginn::HClass const*, huginn::HThread*, HHuginn::values_t&, int );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_MATRIX_HXX_INCLUDED */

