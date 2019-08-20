/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "matrix.hxx"
#include "runtime.hxx"
#include "builtin.hxx"
#include "helper.hxx"
#include "thread.hxx"
#include "objectfactory.hxx"
#include "instruction.hxx"
#include "hcore/safe_int.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

HMatrix::HMatrix( huginn::HThread* thread_, HClass const* class_, HHuginn::values_t& values_, int position_ )
	: HValue( class_ )
	, _data() {
	char const name[] = "Matrix.constructor";
	verify_arg_count( name, values_, 1, meta::max_signed<int>::value, thread_, position_ );
	if ( values_[0]->type_id() != HHuginn::TYPE::LIST ) {
		verify_signature( name, values_, { HHuginn::TYPE::FUNCTION_REFERENCE, HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER }, thread_, position_ );
		huginn::HFunctionReference const& fr( *static_cast<huginn::HFunctionReference const*>( values_[0].raw() ) );
		try {
			int myRows( safe_int::cast<int>( get_integer( values_[1] ) ) );
			if ( myRows < 1 ) {
				throw HHuginn::HHuginnRuntimeException(
					"Invalid number of rows in matrix specification: "_ys.append( myRows ).append( "." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
			int cols( safe_int::cast<int>( get_integer( values_[2] ) ) );
			if ( cols < 1 ) {
				throw HHuginn::HHuginnRuntimeException(
					"Invalid number of columns in matrix specification: "_ys.append( cols ).append( "." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
			if ( fr.function().id() == bit_cast<void const*>( &builtin::number ) ) {
				_data = data_t( make_resource<arbitrary_precision_matrix_t>( myRows, cols ) );
			} else if ( fr.function().id() == bit_cast<void const*>( &builtin::real ) ) {
				_data = data_t( make_resource<floating_point_matrix_t>( myRows, cols ) );
			} else {
				throw HHuginn::HHuginnRuntimeException(
					"Bad matrix type: `"_ys.append( thread_->runtime().function_name( fr.function().id() ) ).append( "`." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
		} catch ( hcore::HException const& e ) {
			throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
		}
	} else {
		huginn::HList::values_t const* rowData( nullptr );
		HHuginn::type_id_t t( static_cast<int>( HHuginn::TYPE::UNKNOWN ) );
		int myRows( 0 );
		int cols( 0 );
		try {
			myRows = safe_int::cast<int>( values_.get_size() );
			cols = safe_int::cast<int>( ( rowData = &get_list( values_[0] ) )->get_size() );
			if ( cols < 1 ) {
				throw HHuginn::HHuginnRuntimeException( "Invalid number of columns in matrix specification: "_ys.append( cols ).append( "." ), thread_->current_frame()->file_id(), position_ );
			}
			t = (*rowData)[0]->type_id();
			if ( t == HHuginn::TYPE::NUMBER ) {
				_data = data_t( make_resource<arbitrary_precision_matrix_t>( myRows, cols ) );
			} else if ( t == HHuginn::TYPE::REAL ) {
				_data = data_t( make_resource<floating_point_matrix_t>( myRows, cols ) );
			} else {
				throw HHuginn::HHuginnRuntimeException( "Matrix must have numeric data, either `number` or `real`.", thread_->current_frame()->file_id(), position_ );
			}
		} catch ( hcore::HException const& e ) {
			throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
		}
		for ( int r( 0 ); r < myRows; ++ r ) {
			verify_arg_type( name, values_, r, HHuginn::TYPE::LIST, ARITY::MULTIPLE, thread_, position_ );
			int otherCols( 0 );
			if ( ( otherCols = static_cast<int>( ( rowData = &get_list( values_[r] ) )->get_size() ) ) != cols ) {
				throw HHuginn::HHuginnRuntimeException(
					"Inconsistent number of columns across rows: "_ys.append( otherCols ).append( " vs " ).append( cols ).append( "." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
			for ( int c( 0 ); c < cols; ++ c ) {
				HHuginn::value_t const& v( (*rowData)[c] );
				if ( v->type_id() != t ) {
					throw HHuginn::HHuginnRuntimeException(
						"Non uniformly typed data in matrix definition, in row: "_ys.append( r ).append( ", column: " ).append( c ).append( "." ),
						thread_->current_frame()->file_id(),
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

HMatrix::HMatrix( HClass const* class_, arbitrary_precision_matrix_ptr_t&& data_ )
	: HValue( class_ )
	, _data( yaal::move( data_ ) ) {
}

HMatrix::HMatrix( HClass const* class_, floating_point_matrix_ptr_t&& data_ )
	: HValue( class_ )
	, _data( yaal::move( data_ ) ) {
}

HHuginn::value_t HMatrix::columns( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.columns";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HHuginn::value_t v;
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_integer( m.columns() );
	} else {
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_integer( m.columns() );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HMatrix::rows( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.rows";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HHuginn::value_t v;
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_integer( m.rows() );
	} else {
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		v = thread_->runtime().object_factory()->create_integer( m.rows() );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HMatrix::get( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	verify_signature( "Matrix.get", values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER }, thread_, position_ );
	int row( static_cast<int>( get_integer( values_[0] ) ) );
	int col( static_cast<int>( get_integer( values_[1] ) ) );
	HHuginn::value_t v;
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		if ( ( row < 0 ) || ( row >= m.rows() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad row: "_ys.append( row ), thread_->current_frame()->file_id(), position_ );
		}
		if ( ( col < 0 ) || ( col >= m.columns() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad column: "_ys.append( col ), thread_->current_frame()->file_id(), position_ );
		}
		v = thread_->runtime().object_factory()->create_number( m[row][col] );
	} else {
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		if ( ( row < 0 ) || ( row >= m.rows() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad row: "_ys.append( row ), thread_->current_frame()->file_id(), position_ );
		}
		if ( ( col < 0 ) || ( col >= m.columns() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad column: "_ys.append( col ), thread_->current_frame()->file_id(), position_ );
		}
		v = thread_->runtime().object_factory()->create_real( m[row][col] );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HMatrix::set( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.set";
	verify_signature( name, values_, { HHuginn::TYPE::INTEGER, HHuginn::TYPE::INTEGER, HHuginn::TYPE::UNKNOWN }, thread_, position_ );
	int row( static_cast<int>( get_integer( values_[0] ) ) );
	int col( static_cast<int>( get_integer( values_[1] ) ) );
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		verify_arg_type( name, values_, 2, HHuginn::TYPE::NUMBER, ARITY::MULTIPLE, thread_, position_ );
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		if ( ( row < 0 ) || ( row >= m.rows() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad row: "_ys.append( row ), thread_->current_frame()->file_id(), position_ );
		}
		if ( ( col < 0 ) || ( col >= m.columns() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad column: "_ys.append( col ), thread_->current_frame()->file_id(), position_ );
		}
		m[row][col] = get_number( values_[2] );
	} else {
		verify_arg_type( name, values_, 2, HHuginn::TYPE::REAL, ARITY::MULTIPLE, thread_, position_ );
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		if ( ( row < 0 ) || ( row >= m.rows() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad row: "_ys.append( row ), thread_->current_frame()->file_id(), position_ );
		}
		if ( ( col < 0 ) || ( col >= m.columns() ) ) {
			throw HHuginn::HHuginnRuntimeException( "Bad column: "_ys.append( col ), thread_->current_frame()->file_id(), position_ );
		}
		m[row][col] = get_real( values_[2] );
	}
	return ( *object_ );
	M_EPILOG
}

void HMatrix::do_operator_add( HThread* thread_, HHuginn::value_t&, HHuginn::value_t const& other_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v;
	HMatrix const* arg( static_cast<HMatrix const*>( other_.raw() ) );
	if ( _data.type() != arg->_data.type() ) {
		throw HHuginn::HHuginnRuntimeException( "Non matching data types.", thread_->current_frame()->file_id(), position_ );
	}
	try {
		if ( _data.type() == 0 ) {
			arbitrary_precision_matrix_t& m( *( _data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			arbitrary_precision_matrix_t const& ma( *( arg->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			m += ma;
		} else {
			floating_point_matrix_t& m( *( _data.get<floating_point_matrix_ptr_t>().raw() ) );
			floating_point_matrix_t const& ma( *( arg->_data.get<floating_point_matrix_ptr_t>().raw() ) );
			m += ma;
		}
	} catch ( hcore::HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return;
	M_EPILOG
}

void HMatrix::do_operator_subtract( HThread* thread_, HHuginn::value_t&, HHuginn::value_t const& other_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v;
	HMatrix const* arg( static_cast<HMatrix const*>( other_.raw() ) );
	if ( _data.type() != arg->_data.type() ) {
		throw HHuginn::HHuginnRuntimeException( "Non matching data types.", thread_->current_frame()->file_id(), position_ );
	}
	try {
		if ( _data.type() == 0 ) {
			arbitrary_precision_matrix_t& m( *( _data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			arbitrary_precision_matrix_t const& ma( *( arg->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			m -= ma;
		} else {
			floating_point_matrix_t& m( *( _data.get<floating_point_matrix_ptr_t>().raw() ) );
			floating_point_matrix_t const& ma( *( arg->_data.get<floating_point_matrix_ptr_t>().raw() ) );
			m -= ma;
		}
	} catch ( hcore::HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return;
	M_EPILOG
}

void HMatrix::do_operator_multiply( HThread* thread_, HHuginn::value_t&, HHuginn::value_t const& other_, int position_ ) {
	M_PROLOG
	HHuginn::value_t v;
	HMatrix const* arg( static_cast<HMatrix const*>( other_.raw() ) );
	if ( _data.type() != arg->_data.type() ) {
		throw HHuginn::HHuginnRuntimeException( "Non matching data types.", thread_->current_frame()->file_id(), position_ );
	}
	try {
		if ( _data.type() == 0 ) {
			arbitrary_precision_matrix_t& m( *( _data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			arbitrary_precision_matrix_t const& ma( *( arg->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			m *= ma;
		} else {
			floating_point_matrix_t& m( *( _data.get<floating_point_matrix_ptr_t>().raw() ) );
			floating_point_matrix_t const& ma( *( arg->_data.get<floating_point_matrix_ptr_t>().raw() ) );
			m *= ma;
		}
	} catch ( hcore::HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return;
	M_EPILOG
}

HHuginn::value_t HMatrix::do_operator_modulus( HThread* thread_, HHuginn::value_t const&, int position_ ) const {
	M_PROLOG
	HHuginn::value_t v;
	try {
		if ( _data.type() == 0 ) {
			arbitrary_precision_matrix_t const& m( *( _data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			v = thread_->runtime().object_factory()->create_number( m.det() );
		} else {
			floating_point_matrix_t const& m( *( _data.get<floating_point_matrix_ptr_t>().raw() ) );
			v = thread_->runtime().object_factory()->create_real( m.det() );
		}
	} catch ( hcore::HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HMatrix::scale( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.scale";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::NUMBER, ARITY::UNARY, thread_, position_ );
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		m *= get_number( values_[0] );
	} else {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::REAL, ARITY::UNARY, thread_, position_ );
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		m *= get_real( values_[0] );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HMatrix::scale_to( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.scale_to";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::NUMBER, ARITY::UNARY, thread_, position_ );
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		hcore::HNumber extremum;
		for ( int r( 0 ), rows( m.rows() ), cols( m.columns() ); r < rows; ++ r ) {
			for ( int c( 0 ); c < cols; ++ c ) {
				hcore::HNumber n( m[r][c] );
				n.abs();
				if ( n > extremum ) {
					extremum = n;
				}
			}
		}
		if ( extremum == number::N0 ) {
			throw HHuginn::HHuginnRuntimeException( "Zeroed matrix cannot be scaled.", thread_->current_frame()->file_id(), position_ );
		}
		m *= get_number( values_[0] ) / extremum;
	} else {
		verify_arg_type( name, values_, 0, HHuginn::TYPE::REAL, ARITY::UNARY, thread_, position_ );
		floating_point_matrix_t& m( *( o->_data.get<floating_point_matrix_ptr_t>().raw() ) );
		double long extremum( 0.L );
		for ( int r( 0 ), rows( m.rows() ), cols( m.columns() ); r < rows; ++ r ) {
			for ( int c( 0 ); c < cols; ++ c ) {
				if ( math::abs( m[r][c] ) > extremum ) {
					extremum = math::abs( m[r][c] );
				}
			}
		}
		if ( extremum == 0.L ) {
			throw HHuginn::HHuginnRuntimeException( "Zeroed matrix cannot be scaled.", thread_->current_frame()->file_id(), position_ );
		}
		m *= get_real( values_[0] ) / extremum;
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HMatrix::inverse( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.inverse";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	HHuginn::value_t v;
	try {
		if ( o->_data.type() == 0 ) {
			v = thread_->object_factory().create<HMatrix>( o->HValue::get_class(), make_resource<arbitrary_precision_matrix_t>( o->_data.get<arbitrary_precision_matrix_ptr_t>()->inverse() ) );
		} else {
			v = thread_->object_factory().create<HMatrix>( o->HValue::get_class(), make_resource<floating_point_matrix_t>( o->_data.get<floating_point_matrix_ptr_t>()->inverse() ) );
		}
	} catch ( hcore::HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return ( v );
	M_EPILOG
}

HHuginn::value_t HMatrix::do_operator_negate( HThread* thread_, HHuginn::value_t const&, int position_ ) const {
	M_PROLOG
	HHuginn::value_t v;
	try {
		if ( _data.type() == 0 ) {
			v = thread_->object_factory().create<HMatrix>( HValue::get_class(), make_resource<arbitrary_precision_matrix_t>( -*_data.get<arbitrary_precision_matrix_ptr_t>() ) );
		} else {
			v = thread_->object_factory().create<HMatrix>( HValue::get_class(), make_resource<floating_point_matrix_t>( -*_data.get<floating_point_matrix_ptr_t>() ) );
		}
	} catch ( hcore::HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return ( v );
	M_EPILOG
}

bool HMatrix::do_operator_equals( HThread* thread_, HHuginn::value_t const&, HHuginn::value_t const& other_, int position_ ) const {
	bool res( false );
	HMatrix const* arg( static_cast<HMatrix const*>( other_.raw() ) );
	if ( _data.type() != arg->_data.type() ) {
		throw HHuginn::HHuginnRuntimeException( "Non matching data types.", thread_->current_frame()->file_id(), position_ );
	}
	try {
		if ( _data.type() == 0 ) {
			arbitrary_precision_matrix_t const& m( *( _data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			arbitrary_precision_matrix_t const& ma( *( arg->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
			res = m == ma;
		} else {
			floating_point_matrix_t const& m( *( _data.get<floating_point_matrix_ptr_t>().raw() ) );
			floating_point_matrix_t const& ma( *( arg->_data.get<floating_point_matrix_ptr_t>().raw() ) );
			res = m == ma;
		}
	} catch ( hcore::HException const& e ) {
		throw HHuginn::HHuginnRuntimeException( e.what(), thread_->current_frame()->file_id(), position_ );
	}
	return ( res );
}

HHuginn::value_t HMatrix::transpose( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.transpose";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	HHuginn::value_t v;
	if ( o->_data.type() == 0 ) {
		v = thread_->object_factory().create<HMatrix>( o->HValue::get_class(), make_resource<arbitrary_precision_matrix_t>( o->_data.get<arbitrary_precision_matrix_ptr_t>()->transpose() ) );
	} else {
		v = thread_->object_factory().create<HMatrix>( o->HValue::get_class(), make_resource<floating_point_matrix_t>( o->_data.get<floating_point_matrix_ptr_t>()->transpose() ) );
	}
	return ( v );
	M_EPILOG
}

template<typename value_t, typename matrix_t, typename call_t>
void matrix_apply( HHuginn::value_t store_, huginn::HThread* thread_, matrix_t& matrix_, call_t& call_, int position_ ) {
	HHuginn::value_t rowHolder( thread_->object_factory().create_integer( 0 ) );
	HHuginn::value_t colHolder( thread_->object_factory().create_integer( 0 ) );
	huginn::HInteger& row( *static_cast<huginn::HInteger*>( rowHolder.raw() ) );
	huginn::HInteger& col( *static_cast<huginn::HInteger*>( colHolder.raw() ) );
	value_t& val( *static_cast<value_t*>( store_.raw() ) );
	HHuginn::value_t res;
	HHuginn::type_id_t expectedType( store_->type_id() );
	for ( int r( 0 ), ROWS( matrix_.rows() ), COLS( matrix_.columns() ); r < ROWS; ++ r ) {
		for ( int c( 0 ); c < COLS; ++ c ) {
			row.value() = r;
			col.value() = c;
			val.value() = matrix_[r][c];
			res = call_->operator_call( thread_, call_, HArguments( thread_, store_, colHolder, rowHolder ), position_ );
			if ( res->type_id() != expectedType ) {
				throw HHuginn::HHuginnRuntimeException(
					"Applied transformation function shall return `"_ys
						.append( store_->get_class()->name() )
						.append( "`, but result was a `" )
						.append( res->get_class()->name() )
						.append( "` instead." ),
					thread_->current_frame()->file_id(),
					position_
				);
			}
			matrix_[r][c] = static_cast<value_t*>( res.raw() )->value();
		}
	}
}

HHuginn::value_t HMatrix::apply( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.apply";
	verify_arg_count( name, values_, 1, 1, thread_, position_ );
	verify_arg_callable( name, values_, 0, ARITY::UNARY, thread_, position_ );
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	HHuginn::value_t v;
	if ( o->_data.type() == 0 ) {
		arbitrary_precision_matrix_t& m( *o->_data.get<arbitrary_precision_matrix_ptr_t>() );
		matrix_apply<HNumber>( thread_->object_factory().create_number( 0 ), thread_, m, values_[0], position_ );
	} else {
		floating_point_matrix_t& m( *o->_data.get<floating_point_matrix_ptr_t>() );
		matrix_apply<HReal>( thread_->object_factory().create_real( 0 ), thread_, m, values_[0], position_ );
	}
	return ( *object_ );
	M_EPILOG
}

HHuginn::value_t HMatrix::to_string( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
	M_PROLOG
	char const name[] = "Matrix.to_string";
	verify_arg_count( name, values_, 0, 0, thread_, position_ );
	hcore::HString s( full_class_name( thread_->runtime(), *object_ ) );
	s.append( "(" );
	HMatrix* o( static_cast<HMatrix*>( object_->raw() ) );
	if ( o->_data.type() == 0 ) {
		arbitrary_precision_matrix_t& m( *( o->_data.get<arbitrary_precision_matrix_ptr_t>().raw() ) );
		for ( int r( 0 ), rows( m.rows() ), cols( m.columns() ); r < rows; ++ r ) {
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
		for ( int r( 0 ), rows( m.rows() ), cols( m.columns() ); r < rows; ++ r ) {
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
	return ( thread_->runtime().object_factory()->create_string( yaal::move( s ) ) );
	M_EPILOG
}

HHuginn::value_t HMatrix::create_instance( HClass const* class_, huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
	return ( thread_->object_factory().create<HMatrix>( thread_, class_, values_, position_ ) );
}

HHuginn::class_t HMatrix::get_class( HRuntime* runtime_, HClass const* origin_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Matrix",
			"The `Matrix` class provides mathematical concept of number matrices. It supports operations of addition, multiplication, subtraction, scaling, inversion and transposition.",
			HHuginn::ACCESS::PUBLIC,
			HClass::TYPE::BUILTIN,
			origin_,
			&HMatrix::create_instance
		)
	);
	HHuginn::field_definitions_t fd{
		{ "columns",   runtime_->create_method( &HMatrix::columns ),   "get column dimension of this `Matrix`" },
		{ "rows",      runtime_->create_method( &HMatrix::rows ),      "get row dimension of this `Matrix`" },
		{ "get",       runtime_->create_method( &HMatrix::get ),       "( *row*, *column* ) - get numeric value present at given *row* and *column* in this `Matrix`" },
		{ "set",       runtime_->create_method( &HMatrix::set ),       "( *row*, *column*, *value* ) - set given numeric *value* at given *row* and *column* in this `Matrix`" },
		{ "scale",     runtime_->create_method( &HMatrix::scale ),     "( *factor* ) - scale all values in this `Matrix` by given *factor*" },
		{ "scale_to",  runtime_->create_method( &HMatrix::scale_to ),  "( *cap* ) - rescale values in this `Matrix` so maximum of its values is equal to *cap*" },
		{ "inverse",   runtime_->create_method( &HMatrix::inverse ),   "find *inverse* of this `Matrix`" },
		{ "transpose", runtime_->create_method( &HMatrix::transpose ), "create transposed version of this `Matrix`" },
		{ "apply",     runtime_->create_method( &HMatrix::apply ),     "( *fun* ) - apply unary function *fun* over all values in this `Matrix`" },
		{ "to_string", runtime_->create_method( &HMatrix::to_string ), "get string representation of this `Matrix`" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( c );
	M_EPILOG
}

HHuginn::value_t HMatrix::do_clone( huginn::HThread* thread_, HHuginn::value_t*, int ) const {
	M_PROLOG
	HHuginn::value_t v;
	if ( _data.type() == 0 ) {
		v = thread_->object_factory().create<HMatrix>( HValue::get_class(), make_resource<arbitrary_precision_matrix_t>( *( _data.get<arbitrary_precision_matrix_ptr_t>() ) ) );
	} else {
		v = thread_->object_factory().create<HMatrix>( HValue::get_class(), make_resource<floating_point_matrix_t>( *( _data.get<floating_point_matrix_ptr_t>() ) ) );
	}
	return ( v );
	M_EPILOG
}

}

}

}

