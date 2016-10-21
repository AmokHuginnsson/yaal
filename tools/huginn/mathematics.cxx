/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  mathematics.cxx - this file is integral part of `yaal' project.

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

#include <cmath>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcore/math.hxx"
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "tools/huginn/thread.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "objectfactory.hxx"
#include "packagefactory.hxx"
#include "matrix.hxx"
#include "numbersetstatistics.hxx"
#include "randomizer.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HMathematics : public HHuginn::HObject {
	typedef yaal::hcore::HNumber const& ( *constant_getter_t )( yaal::hcore::HNumber::integer_t );
	HHuginn::class_t _matrixClass;
	HHuginn::class_t _numberSetStatisticsClass;
	HHuginn::class_t _randomizerClass;
	HHuginn::class_t _exceptionClass;
public:
	HMathematics( HHuginn::HClass* class_ )
		: HObject( class_ )
		, _matrixClass( HMatrix::get_class( class_->runtime() ) )
		, _numberSetStatisticsClass( HNumberSetStatistics::get_class( class_->runtime() ) )
		, _randomizerClass( HRandomizer::get_class( class_->runtime() ) )
		, _exceptionClass( exception::create_class( class_->runtime(), "MathematicsException" ) ) {
		return;
	}
	static HHuginn::value_t get_constant( char const* name_, constant_getter_t constantGetter_, double long real_, huginn::HThread* thread_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		HString name( "Mathematics." );
		name.append( name_ );
		verify_arg_count( name, values_, 1, 2, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::FUNCTION_REFERENCE, false, position_ );
		HHuginn::HFunctionReference const& fr( *static_cast<HHuginn::HFunctionReference const*>( values_[0].raw() ) );
		int precision( 0 );
		if ( values_.get_size() > 1 ) {
			verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, false, position_ );
			precision = static_cast<int>( get_integer( values_[1] ) );
		}
		if ( precision < 0 ) {
			throw HHuginn::HHuginnRuntimeException( "Bad precision requested: "_ys.append( precision ), position_ );
		}
		HHuginn::value_t v;
		if ( fr.function().id() == bit_cast<void const*>( &huginn_builtin::number ) ) {
			v = thread_->object_factory().create_number( constantGetter_( precision ) );
		} else if ( fr.function().id() == bit_cast<void const*>( &huginn_builtin::real ) ) {
			v = thread_->object_factory().create_real( real_ );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				name.append( " can be either `number' or `real' type, not a `" ).append( thread_->runtime().identifier_name( fr.identifier_id() ) ).append( "'." ),
				position_
			);
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t pi( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		return ( get_constant( "pi", &number::PI, math::PI, thread_, values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t e( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		return ( get_constant( "e", &number::E, math::E, thread_, values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t square_root( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.square_root";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v( thread_->runtime().none_value() );
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			if ( val >= number::N0 ) {
				v = thread_->object_factory().create_number( math::square_root( val ) );
			}
		} else {
			double long val( get_real( values_[0] ) );
			if ( val >= 0 ) {
				v = thread_->object_factory().create_real( math::square_root( val ) );
			}
		}
		if ( v == thread_->runtime().none_value() ) {
			thread_->raise( static_cast<HMathematics*>( object_->raw() )->_exceptionClass.raw(), "bad domain", position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t natural_expotential( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.natural_expotential";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::natural_expotential( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::natural_expotential( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t natural_logarithm( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.natural_logarithm";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v( thread_->runtime().none_value() );
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			if ( val > number::N0 ) {
				v = thread_->object_factory().create_number( math::natural_logarithm( val ) );
			}
		} else {
			double long val( get_real( values_[0] ) );
			if ( val > 0 ) {
				v = thread_->object_factory().create_real( math::natural_logarithm( val ) );
			}
		}
		if ( v == thread_->runtime().none_value() ) {
			thread_->raise( static_cast<HMathematics*>( object_->raw() )->_exceptionClass.raw(), "bad domain", position_ );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t sinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.sinus";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::sinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::sinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t cosinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.cosinus";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::cosinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::cosinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t tangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.tangens";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::tangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::tangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t cotangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.tangens";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::cotangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::cotangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t arcus_sinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.arcus_sinus";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::arcus_sinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::arcus_sinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t arcus_cosinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.arcus_cosinus";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::arcus_cosinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::arcus_cosinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t arcus_tangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.arcus_tangens";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::arcus_tangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::arcus_tangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t arcus_cotangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.arcus_cotangens";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::arcus_cotangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::arcus_cotangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t hyperbolic_sinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.hyperbolic_sinus";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::hyperbolic_sinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::hyperbolic_sinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t hyperbolic_cosinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.hyperbolic_cosinus";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::hyperbolic_cosinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::hyperbolic_cosinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t hyperbolic_tangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.hyperbolic_tangens";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::hyperbolic_tangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::hyperbolic_tangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t hyperbolic_cotangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.hyperbolic_cotangens";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::hyperbolic_cotangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::hyperbolic_cotangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t sigmoid( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.sigmoid";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::sigmoid( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::sigmoid( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t round( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.round";
		verify_arg_count( name, values_, 1, 2, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, false, position_ ) );
		int to( 0 );
		if ( values_.get_size() > 1 ) {
			verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, false, position_ );
			to = static_cast<int>( get_integer( values_[1] ) );
		}
		HHuginn::value_t v( thread_->runtime().none_value() );
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			try {
				v = thread_->object_factory().create_number( val.round( to ) );
			} catch ( HNumberException const& e ) {
				thread_->raise( static_cast<HMathematics*>( object_->raw() )->_exceptionClass.raw(), e.what(), position_ );
			}
		} else {
			if ( to == 0 ) {
				double long val( get_real( values_[0] ) );
				v = thread_->object_factory().create_real( roundl( val ) );
			} else {
				thread_->raise( static_cast<HMathematics*>( object_->raw() )->_exceptionClass.raw(), "rounding to nth place on real is not supported", position_ );
			}
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t floor( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.floor";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v( thread_->runtime().none_value() );
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( val.floor() );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( floorl( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t ceil( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.ceil";
		verify_arg_count( name, values_, 1, 1, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, true, position_ ) );
		HHuginn::value_t v( thread_->runtime().none_value() );
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( val.ceil() );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( ceill( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t differs_at( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.differs_at";
		verify_arg_count( name, values_, 2, 2, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::NUMBER, false, position_ );
		verify_arg_type( name, values_, 1, HHuginn::TYPE::NUMBER, false, position_ );
		HHuginn::value_t v( thread_->runtime().none_value() );
		return ( thread_->object_factory().create_integer( number::differs_at( get_number( values_[0] ), get_number( values_[1] ) ) ) );
		M_EPILOG
	}
	static HHuginn::value_t matrix( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		HMathematics* m( static_cast<HMathematics*>( object_->raw() ) );
		return ( make_pointer<HMatrix>( thread_, m->_matrixClass.raw(), values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t statistics( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		HMathematics* m( static_cast<HMathematics*>( object_->raw() ) );
		return ( make_pointer<HNumberSetStatistics>( m->_numberSetStatisticsClass.raw(), values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t randomizer( huginn::HThread*, HHuginn::value_t* object_, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.randomizer";
		verify_arg_count( name, values_, 0, 1, position_ );
		yaal::u64_t cap( meta::max_unsigned<yaal::u64_t>::value );
		if ( ! values_.is_empty() ) {
			verify_arg_type( name, values_, 0, HHuginn::TYPE::INTEGER, false, position_ );
			cap = static_cast<yaal::u64_t>( get_integer( values_[0] ) );
		}
		HMathematics* m( static_cast<HMathematics*>( object_->raw() ) );
		return ( make_pointer<huginn::HRandomizer>( m->_randomizerClass.raw(), cap ) );
		M_EPILOG
	}
};

namespace package_factory {

class HMathematicsCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} mathematicsCreator;

HHuginn::value_t HMathematicsCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Mathematics",
			nullptr,
			HHuginn::field_definitions_t{
				{ "pi",                   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::pi, _1, _2, _3, _4 ) ), "( *type* [, *precision*] ) - get value of $\\pi$ of type *type*, potentially with given *precision*" },
				{ "e",                    make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::e, _1, _2, _3, _4 ) ), "( *type* [, *precision*] ) - get value of $e$ of type *type*, potentially with given *precision*" },
				{ "square_root",          make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::square_root, _1, _2, _3, _4 ) ), "( *value* ) - calculate square root of given *value*" },
				{ "natural_expotential",  make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::natural_expotential, _1, _2, _3, _4 ) ), "( *x* ) - calculate value of $e^x$ (value of natural expotential of *x*)" },
				{ "natural_logarithm",    make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::natural_logarithm, _1, _2, _3, _4 ) ), "( *x* ) - find natural logarithm of value *x*" },
				{ "sinus",                make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::sinus, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *sine* function of *arg* argument" },
				{ "cosinus",              make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::cosinus, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *cosine* function of *arg* argument" },
				{ "tangens",              make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::tangens, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *tangent* function of *arg* argument" },
				{ "cotangens",            make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::cotangens, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *cotangent* function of *arg* argument" },
				{ "arcus_sinus",          make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::arcus_sinus, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *arcus sine* function of *arg* argument" },
				{ "arcus_cosinus",        make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::arcus_cosinus, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *arcus cosine* function of *arg* argument" },
				{ "arcus_tangens",        make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::arcus_tangens, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *arcus tangent* function of *arg* argument" },
				{ "arcus_cotangens",      make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::arcus_cotangens, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *arcus cotangent* function of *arg* argument" },
				{ "hyperbolic_sinus",     make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::hyperbolic_sinus, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *hyperbolic sine* function of *arg* argument" },
				{ "hyperbolic_cosinus",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::hyperbolic_cosinus, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *hyperbolic cosine* function of *arg* argument" },
				{ "hyperbolic_tangens",   make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::hyperbolic_tangens, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *hyperbolic tangent* function of *arg* argument" },
				{ "hyperbolic_cotangens", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::hyperbolic_cotangens, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *hyperbolic cotangent* function of *arg* argument" },
				{ "sigmoid",              make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::sigmoid, _1, _2, _3, _4 ) ), "( *arg* ) - calculate value of *sigmoid* function of *arg* argument" },
				{ "round",                make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::round, _1, _2, _3, _4 ) ), "( *value*[, *precision*] ) - round *value*, potentially to given *precision*" },
				{ "floor",                make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::floor, _1, _2, _3, _4 ) ), "( *value* ) - get largest integral value not greater than *value*" },
				{ "ceil",                 make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::ceil, _1, _2, _3, _4 ) ), "( *value* ) - get smallest integral value not less than *value*" },
				{ "differs_at",           make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::differs_at, _1, _2, _3, _4 ) ), "( *first*, *second* ) - tell at which decimal position *first* and *second* values have first occurrence of different digit" },
				{ "matrix",               make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::matrix, _1, _2, _3, _4 ) ), "( *type*, *rows*, *cols* ) - create instance of Matrix class of values of type *type* and *rows* rows and *cols* columns" },
				{ "statistics",           make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::statistics, _1, _2, _3, _4 ) ), "( *iterable* ) - calculate numerical statistics over given iterable *iterable* of uniformly types values" },
				{ "randomizer",           make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HMathematics::randomizer, _1, _2, _3, _4 ) ), "([ *cap* ]) - create random number generator which output values are capped at *cap*" }
			},
			"The `Mathematics` package provides essential mathematical functions and classes. All of those functions and classes work with values of both `real` and `number` types."
		)
	);
	runtime_->huginn()->register_class( c );
	return ( make_pointer<HMathematics>( c.raw() ) );
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "Mathematics", &mathematicsCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

