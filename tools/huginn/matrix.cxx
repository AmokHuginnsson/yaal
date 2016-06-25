/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  matrix.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "matrix.hxx"
#include "runtime.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "value_builtin.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HMatrix::HMatrix( huginn::HThread* thread_, HHuginn::HClass const* class_, HHuginn::values_t const& values_, int position_ )
	: HObject( class_ )
	, _data() {
	char const name[] = "Matrix.constructor";
	verify_arg_count( name, values_, 1, meta::max_signed<int>::value, position_ );
	if ( values_[0]->type_id() != HHuginn::TYPE::LIST ) {
		verify_arg_count( name, values_, 1, 3, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::FUNCTION_REFERENCE, false, position_ );
		verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, false, position_ );
		verify_arg_type( name, values_, 2, HHuginn::TYPE::INTEGER, false, position_ );
		HHuginn::HFunctionReference const& fr( *static_cast<HHuginn::HFunctionReference const*>( values_[0].raw() ) );
		int rows( static_cast<int>( get_integer( values_[1] ) ) );
		if ( rows < 1 ) {
			throw HHuginn::HHuginnRuntimeException( "Invalid number of rows in matrix specification: "_ys.append( rows ).append( "." ), position_ );
		}
		int cols( static_cast<int>( get_integer( values_[2] ) ) );
		if ( cols < 1 ) {
			throw HHuginn::HHuginnRuntimeException( "Invalid number of columns in matrix specification: "_ys.append( cols ).append( "." ), position_ );
		}
		if ( fr.function().id() == bit_cast<void const*>( &huginn_builtin::number ) ) {
			_data = data_t( make_resource<arbitrary_precision_matrix_t>( rows, cols ) );
		} else if ( fr.function().id() == bit_cast<void const*>( &huginn_builtin::real ) ) {
			_data = data_t( make_resource<floating_point_matrix_t>( rows, cols ) );
		} else {
			throw HHuginn::HHuginnRuntimeException( "Bad matrix type: `"_ys.append( thread_->runtime().function_name( fr.function().id() ) ).append( "'." ), position_ );
		}
	} else {
		int rows( static_cast<int>( values_.get_size() ) );
		HHuginn::HList::values_t const* rowData( nullptr );
		int cols( static_cast<int>( ( rowData = &get_list( values_[0] ) )->get_size() ) );
		if ( cols < 1 ) {
			throw HHuginn::HHuginnRuntimeException( "Invalid number of columns in matrix specification: "_ys.append( cols ).append( "." ), position_ );
		}
		HHuginn::type_id_t t( (*rowData)[0]->type_id() );
		if ( t == HHuginn::TYPE::NUMBER ) {
			_data = data_t( make_resource<arbitrary_precision_matrix_t>( rows, cols ) );
		} else if ( t == HHuginn::TYPE::REAL ) {
			_data = data_t( make_resource<floating_point_matrix_t>( rows, cols ) );
		} else {
			throw HHuginn::HHuginnRuntimeException( "Matrix must have numeric data, either `number' or `real'.", position_ );
		}
		for ( int r( 0 ); r < rows; ++ r ) {
			verify_arg_type( name, values_, r, HHuginn::TYPE::LIST, false, position_ );
			int otherCols( 0 );
			if ( ( otherCols = static_cast<int>( ( rowData = &get_list( values_[r] ) )->get_size() ) ) != cols ) {
				throw HHuginn::HHuginnRuntimeException(
					"Inconsistent number of columns across rows: "_ys.append( otherCols ).append( " vs " ).append( cols ).append( "." ),
					position_
				);
			}
			for ( int c( 0 ); c < cols; ++ c ) {
				HHuginn::value_t const& v( (*rowData)[c] );
				if ( v->type_id() != t ) {
					throw HHuginn::HHuginnRuntimeException(
						"Non uniformly typed data in matrix definition, in row: "_ys.append( r ).append( ", column: " ).append( c ).append( "." ),
						position_
					);
				}
				if ( t == HHuginn::TYPE::NUMBER ) {
					(*_data.get<arbitrary_precision_matrix_ptr_t>())[r][c] = get_number( v );
				} else {
					(*_data.get<floating_point_matrix_ptr_t>())[r][c] = get_real( v );
				}
			}
		}
	}
	return;
}

HMatrix::HMatrix( HHuginn::HClass const* class_, arbitrary_precision_matrix_ptr_t&& data_ )
	: HObject( class_ )
	, _data( yaal::move( data_ ) ) {
}

HMatrix::HMatrix( HHuginn::HClass const* class_, floating_point_matrix_ptr_t&& data_ )
	: HObject( class_ )
	, _data( yaal::move( data_ ) ) {
}

HHuginn::value_t HMatrix::get( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.get";
	verify_arg_count( name, values_, 2, 2, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, false, position_ );
	verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, false, position_ );
	int row( static_cast<int>( get_integer( values_[0] ) ) );
	int col( static_cast<int>( get_integer( values_[1] ) ) );
	HHuginn::value_t v;
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		if ( ( row < 0 ) || ( row >= m.row() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad row: "_ys.append( row ), position_ );
		}
		if ( ( col < 0 ) || ( col >= m.col() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad col: "_ys.append( col ), position_ );
		}
		v = thread_->runtime().object_factory()->create_number( m[row][col] );
	} else {
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		if ( ( row < 0 ) || ( row >= m.row() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad row: "_ys.append( row ), position_ );
		}
		if ( ( col < 0 ) || ( col >= m.col() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad col: "_ys.append( col ), position_ );
		}
		v = thread_->runtime().object_factory()->create_real( m[row][col] );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HMatrix::set( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.set";
	verify_arg_count( name, values_, 3, 3, position_ );
	verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, false, position_ );
	verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, false, position_ );
	int row( static_cast<int>( get_integer( values_[0] ) ) );
	int col( static_cast<int>( get_integer( values_[1] ) ) );
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		verify_arg_type( name, values_, 2, HHuginn::TYPE::NUMBER, false, position_ );
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		if ( ( row < 0 ) || ( row >= m.row() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad row: "_ys.append( row ), position_ );
		}
		if ( ( col < 0 ) || ( col >= m.col() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad col: "_ys.append( col ), position_ );
		}
		m[row][col] = get_number( values_[2] );
	} else {
		verify_arg_type( name, values_, 2, HHuginn::TYPE::REAL, false, position_ );
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		if ( ( row < 0 ) || ( row >= m.row() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad row: "_ys.append( row ), position_ );
		}
		if ( ( col < 0 ) || ( col >= m.col() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad col: "_ys.append( col ), position_ );
		}
		m[row][col] = get_real( values_[2] );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HMatrix::add( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.add";
	verify_arg_count( name, values_, 1, 1, position_ );
	verify_arg_type( name, values_, 0, (*object_)->get_class(), true, position_ );
	HHuginn::value_t v;
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	HMatrix const* arg( static_cast<HMatrix const*>( values_[0].raw() ) );
	if ( o->_data.type() != arg->_data.type() ) {
		throw HHuginn::HHuginnRuntimeException( "Non matching data types.", position_ );
	}
	try {
		if ( o->_data.type() == 0 ) {
			arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			arbitrary_precision_matrix_t const& ma( *( arg->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			m += ma;
		} else {
			floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
			floating_point_matrix_t const& ma( *( arg->_data.get<floating_point_matrix_ptr_t>().raw() ) );
			m += ma;
		}
	} catch ( HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HMatrix::substract( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.substract";
	verify_arg_count( name, values_, 1, 1, position_ );
	verify_arg_type( name, values_, 0, (*object_)->get_class(), true, position_ );
	HHuginn::value_t v;
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	HMatrix const* arg( static_cast<HMatrix const*>( values_[0].raw() ) );
	if ( o->_data.type() != arg->_data.type() ) {
		throw HHuginn::HHuginnRuntimeException( "Non matching data types.", position_ );
	}
	try {
		if ( o->_data.type() == 0 ) {
			arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			arbitrary_precision_matrix_t const& ma( *( arg->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			m -= ma;
		} else {
			floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
			floating_point_matrix_t const& ma( *( arg->_data.get<floating_point_matrix_ptr_t>().raw() ) );
			m -= ma;
		}
	} catch ( HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HMatrix::multiply( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.multiply";
	verify_arg_count( name, values_, 1, 1, position_ );
	verify_arg_type( name, values_, 0, (*object_)->get_class(), true, position_ );
	HHuginn::value_t v;
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	HMatrix const* arg( static_cast<HMatrix const*>( values_[0].raw() ) );
	if ( o->_data.type() != arg->_data.type() ) {
		throw HHuginn::HHuginnRuntimeException( "Non matching data types.", position_ );
	}
	try {
		if ( o->_data.type() == 0 ) {
			arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			arbitrary_precision_matrix_t const& ma( *( arg->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			m *= ma;
		} else {
			floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
			floating_point_matrix_t const& ma( *( arg->_data.get<floating_point_matrix_ptr_t>().raw() ) );
			m *= ma;
		}
	} catch ( HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HMatrix::det( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.det";
	verify_arg_count( name, values_, 0, 0, position_ );
	HHuginn::value_t v;
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_number( m.det() );
	} else {
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_real( m.det() );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HMatrix::scale( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.scale";
	verify_arg_count( name, values_, 1, 1, position_ );
	HHuginn::value_t v;
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::NUMBER, true, position_ );
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		m *= get_number( values_[0] );
	} else {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::REAL, true, position_ );
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		m *= get_real( values_[0] );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HMatrix::scale_to( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.scale_to";
	verify_arg_count( name, values_, 1, 1, position_ );
	HHuginn::value_t v;
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::NUMBER, true, position_ );
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		HNumber extream;
		for ( int r( 0 ), rows( m.row() ), cols( m.col() ); r < rows; ++ r ) {
			for ( int c( 0 ); c < cols; ++ c ) {
				HNumber n( m[r][c] );
				n.abs();
				if ( n > extream ) {
					extream = n;
				}
			}
		}
		if ( extream == number::N0 ) {
			throw HHuginn::HHuginnRuntimeException( "Zeroed matrix cannot be scaled.", position_ );
		}
		m *= get_number( values_[0] ) / extream;
	} else {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::REAL, true, position_ );
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		double long extream( 0.L );
		for ( int r( 0 ), rows( m.row() ), cols( m.col() ); r < rows; ++ r ) {
			for ( int c( 0 ); c < cols; ++ c ) {
				if ( abs( m[r][c] ) > extream ) {
					extream = abs( m[r][c] );
				}
			}
		}
		if ( extream == 0.L ) {
			throw HHuginn::HHuginnRuntimeException( "Zeroed matrix cannot be scaled.", position_ );
		}
		m *= get_real( values_[0] ) / extream;
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HMatrix::to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.to_string";
	verify_arg_count( name, values_, 0, 0, position_ );
	HString s( "Matrix(" );
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		for ( int r( 0 ), rows( m.row() ), cols( m.col() ); r < rows; ++ r ) {
			if ( r > 0 ) {
				s.append( ", " );
			}
			s.append( "[" );
			for ( int c( 0 ); c < cols; ++ c ) {
				if ( c > 0 ) {
					s.append( ", " );
				}
				s.append( "$" ).append( m[r][c].to_string() );
			}
			s.append( "]" );
		}
	} else {
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		for ( int r( 0 ), rows( m.row() ), cols( m.col() ); r < rows; ++ r ) {
			if ( r > 0 ) {
				s.append( ", " );
			}
			s.append( "[" );
			for ( int c( 0 ); c < cols; ++ c ) {
				if ( c > 0 ) {
					s.append( ", " );
				}
				s.append( m[r][c] );
			}
			s.append( "]" );
		}
	}
	s.append( ")" );
	return ( thread_->runtime().object_factory()->create_string( s ) );
	M_EPILOG
}

HHuginn::class_t HMatrix::get_class( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Matrix",
			nullptr,
			HHuginn::field_definitions_t{
				{ "get", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMatrix::get, _1, _2, _3, _4 ) ) },
				{ "set", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMatrix::set, _1, _2, _3, _4 ) ) },
				{ "add", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMatrix::add, _1, _2, _3, _4 ) ) },
				{ "substract", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMatrix::substract, _1, _2, _3, _4 ) ) },
				{ "multiply", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMatrix::multiply, _1, _2, _3, _4 ) ) },
				{ "det", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMatrix::det, _1, _2, _3, _4 ) ) },
				{ "scale", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMatrix::scale, _1, _2, _3, _4 ) ) },
				{ "scale_to", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMatrix::scale_to, _1, _2, _3, _4 ) ) },
				{ "to_string", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMatrix::to_string, _1, _2, _3, _4 ) ) }
			}
		)
	);
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HMatrix::do_clone( huginn::HRuntime* ) const {
	M_PROLOG
	HHuginn::value_t v;
	if ( _data.type() == 0 ) {
		v = make_pointer<HMatrix>( HObject::get_class(), make_resource<arbitrary_precision_matrix_t>( *( _data.get<arbitrary_precision_matrix_ptr_t>() ) ) );
	} else {
		v = make_pointer<HMatrix>( HObject::get_class(), make_resource<floating_point_matrix_t>( *( _data.get<floating_point_matrix_ptr_t>() ) ) );
	}
	return ( v );
	M_EPILOG
}

}

}

}

