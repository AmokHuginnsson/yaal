/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
#include "complex.hxx"
#include "numbersetstatistics.hxx"
#include "randomizer.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HMathematics : public HHuginn::HValue {
	typedef yaal::hcore::HNumber const& ( *constant_getter_t )( yaal::hcore::HNumber::integer_t );
	HHuginn::class_t _complexClass;
	HHuginn::class_t _matrixClass;
	HHuginn::class_t _numberSetStatisticsClass;
	HHuginn::class_t _randomizerClass;
	HHuginn::class_t _exceptionClass;
public:
	HMathematics( HHuginn::HClass* class_ )
		: HValue( class_ )
		, _complexClass(
			add_to_package(
				class_,
				HComplex::get_class( class_->runtime(), class_ ),
				"( *real*, *imaginary* ) - create instance of Complex with *real* real part and *imaginary* imaginary part."
			)
		)
		, _matrixClass(
			add_to_package(
				class_,
				HMatrix::get_class( class_->runtime(), class_ ),
				"( *type*, *rows*, *cols* ) - create instance of Matrix class of values of type *type* and *rows* rows and *cols* columns."
			)
		)
		, _numberSetStatisticsClass(
			add_to_package(
				class_,
				HNumberSetStatistics::get_class( class_->runtime(), class_ ),
				"( *iterable* ) - calculate numerical statistics over given iterable *iterable* of uniformly types values"
			)
		)
		, _randomizerClass(
			add_to_package(
				class_,
				HRandomizer::get_class( class_->runtime(), class_ ),
				"([ *cap* ]) - create random number generator which output values are capped at *cap*"
			)
		)
		, _exceptionClass( package_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t get_constant( char const* name_, constant_getter_t constantGetter_, double long real_, huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( name_, values_, 1, 2, thread_, position_ );
		verify_arg_type( name_, values_, 0, HHuginn::TYPE::FUNCTION_REFERENCE, ARITY::MULTIPLE, thread_, position_ );
		HHuginn::HFunctionReference const& fr( *static_cast<HHuginn::HFunctionReference const*>( values_[0].raw() ) );
		int precision( 0 );
		if ( values_.get_size() > 1 ) {
			verify_arg_type( name_, values_, 1, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
			precision = static_cast<int>( get_integer( values_[1] ) );
		}
		if ( precision < 0 ) {
			throw HHuginn::HHuginnRuntimeException( "Bad precision requested: "_ys.append( precision ), thread_->current_frame()->file_id(), position_ );
		}
		HHuginn::value_t v;
		if ( fr.function().id() == bit_cast<void const*>( &huginn_builtin::number ) ) {
			v = thread_->object_factory().create_number( constantGetter_( precision ) );
		} else if ( fr.function().id() == bit_cast<void const*>( &huginn_builtin::real ) ) {
			v = thread_->object_factory().create_real( real_ );
		} else {
			throw HHuginn::HHuginnRuntimeException(
				hcore::to_string( name_ )
					.append( " can be either `number' or `real' type, not a `" )
					.append( thread_->runtime().identifier_name( fr.identifier_id() ) )
					.append( "'." ),
				thread_->current_frame()->file_id(),
				position_
			);
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t pi( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		return ( get_constant( "Mathematics.pi", &number::PI, math::PI, thread_, values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t e( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		return ( get_constant( "Mathematics.e", &number::E, math::E, thread_, values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t square_root( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.square_root";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v( thread_->runtime().none_value() );
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
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
	static HHuginn::value_t natural_exponential( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.natural_exponential";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::natural_exponential( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::natural_exponential( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t natural_logarithm( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.natural_logarithm";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v( thread_->runtime().none_value() );
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
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
	static HHuginn::value_t sinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.sinus";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::sinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::sinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t cosinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.cosinus";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::cosinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::cosinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t tangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.tangens";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::tangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::tangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t cotangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.tangens";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::cotangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::cotangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t arcus_sinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.arcus_sinus";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::arcus_sinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::arcus_sinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t arcus_cosinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.arcus_cosinus";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::arcus_cosinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::arcus_cosinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t arcus_tangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.arcus_tangens";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::arcus_tangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::arcus_tangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t arcus_cotangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.arcus_cotangens";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::arcus_cotangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::arcus_cotangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t hyperbolic_sinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.hyperbolic_sinus";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::hyperbolic_sinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::hyperbolic_sinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t hyperbolic_cosinus( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.hyperbolic_cosinus";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::hyperbolic_cosinus( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::hyperbolic_cosinus( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t hyperbolic_tangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.hyperbolic_tangens";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::hyperbolic_tangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::hyperbolic_tangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t hyperbolic_cotangens( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.hyperbolic_cotangens";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::hyperbolic_cotangens( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::hyperbolic_cotangens( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t sigmoid( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.sigmoid";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::sigmoid( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::sigmoid( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t error_function( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.error_function";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			v = thread_->object_factory().create_number( math::error_function( val ) );
		} else {
			double long val( get_real( values_[0] ) );
			v = thread_->object_factory().create_real( math::error_function( val ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t cumulative_distribution_function( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.cumulative_distribution_function";
		verify_arg_count( name, values_, 3, 3, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::MULTIPLE, thread_, position_ ) );
		verify_signature( name, values_, t == HHuginn::TYPE::REAL ? types_t{ HHuginn::TYPE::REAL, HHuginn::TYPE::REAL, HHuginn::TYPE::REAL } : types_t{ HHuginn::TYPE::NUMBER, HHuginn::TYPE::NUMBER, HHuginn::TYPE::NUMBER, }, thread_, position_ );
		HHuginn::value_t v;
		if ( t == HHuginn::TYPE::NUMBER ) {
			HNumber const& val( get_number( values_[0] ) );
			HNumber const& mu( get_number( values_[1] ) );
			HNumber const& sigma( get_number( values_[2] ) );
			v = thread_->object_factory().create_number( xmath::cumulative_distribution_function( val, mu, sigma ) );
		} else {
			double long val( get_real( values_[0] ) );
			double long mu( get_real( values_[1] ) );
			double long sigma( get_real( values_[2] ) );
			v = thread_->object_factory().create_real( xmath::cumulative_distribution_function( val, mu, sigma ) );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t round( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.round";
		verify_arg_count( name, values_, 1, 2, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::MULTIPLE, thread_, position_ ) );
		int to( 0 );
		if ( values_.get_size() > 1 ) {
			verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
			to = static_cast<int>( get_integer( values_[1] ) );
		}
		if ( to < 0 ) {
			throw HHuginn::HHuginnRuntimeException( "Invalid requested round value: "_ys.append( to ), thread_->current_frame()->file_id(), position_ );
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
			double long val( get_real( values_[0] ) );
			double long order( powl( 10.L, static_cast<double long>( to ) ) );
			val = to == 0 ? roundl( val ) : roundl( val * order ) / order;
			v = thread_->object_factory().create_real( val );
		}
		return ( v );
		M_EPILOG
	}
	static HHuginn::value_t floor( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.floor";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
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
	static HHuginn::value_t ceil( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Mathematics.ceil";
		verify_arg_count( name, values_, 1, 1, thread_, position_ );
		HHuginn::type_id_t t( verify_arg_numeric( name, values_, 0, ARITY::UNARY, thread_, position_ ) );
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
	static HHuginn::value_t differs_at( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Mathematics.differs_at", values_, { HHuginn::TYPE::NUMBER, HHuginn::TYPE::NUMBER }, thread_, position_ );
		HHuginn::value_t v( thread_->runtime().none_value() );
		return ( thread_->object_factory().create_integer( number::differs_at( get_number( values_[0] ), get_number( values_[1] ) ) ) );
		M_EPILOG
	}
	static HHuginn::value_t statistics( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HMathematics* m( static_cast<HMathematics*>( object_->raw() ) );
		return ( m->_numberSetStatisticsClass->create_instance( thread_, object_, values_, position_ ) );
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
			"The `Mathematics` package provides essential mathematical functions and classes. All of those functions and classes work with values of both `real` and `number` types."
		)
	);
	HHuginn::field_definitions_t fd{
		{ "pi",                   runtime_->create_method( c.raw(), &HMathematics::pi ),                   "( *type* [, *precision*] ) - get value of $\\pi$ of type *type*, potentially with given *precision*" },
		{ "e",                    runtime_->create_method( c.raw(), &HMathematics::e ),                    "( *type* [, *precision*] ) - get value of $e$ of type *type*, potentially with given *precision*" },
		{ "square_root",          runtime_->create_method( c.raw(), &HMathematics::square_root ),          "( *value* ) - calculate square root of given *value*" },
		{ "natural_exponential",  runtime_->create_method( c.raw(), &HMathematics::natural_exponential ),  "( *x* ) - calculate value of $e^x$ (value of natural exponential of *x*)" },
		{ "natural_logarithm",    runtime_->create_method( c.raw(), &HMathematics::natural_logarithm ),    "( *x* ) - find natural logarithm of value *x*" },
		{ "sinus",                runtime_->create_method( c.raw(), &HMathematics::sinus ),                "( *arg* ) - calculate value of *sine* function of *arg* argument" },
		{ "cosinus",              runtime_->create_method( c.raw(), &HMathematics::cosinus ),              "( *arg* ) - calculate value of *cosine* function of *arg* argument" },
		{ "tangens",              runtime_->create_method( c.raw(), &HMathematics::tangens ),              "( *arg* ) - calculate value of *tangent* function of *arg* argument" },
		{ "cotangens",            runtime_->create_method( c.raw(), &HMathematics::cotangens ),            "( *arg* ) - calculate value of *cotangent* function of *arg* argument" },
		{ "arcus_sinus",          runtime_->create_method( c.raw(), &HMathematics::arcus_sinus ),          "( *arg* ) - calculate value of *arcus sine* function of *arg* argument" },
		{ "arcus_cosinus",        runtime_->create_method( c.raw(), &HMathematics::arcus_cosinus ),        "( *arg* ) - calculate value of *arcus cosine* function of *arg* argument" },
		{ "arcus_tangens",        runtime_->create_method( c.raw(), &HMathematics::arcus_tangens ),        "( *arg* ) - calculate value of *arcus tangent* function of *arg* argument" },
		{ "arcus_cotangens",      runtime_->create_method( c.raw(), &HMathematics::arcus_cotangens ),      "( *arg* ) - calculate value of *arcus cotangent* function of *arg* argument" },
		{ "hyperbolic_sinus",     runtime_->create_method( c.raw(), &HMathematics::hyperbolic_sinus ),     "( *arg* ) - calculate value of *hyperbolic sine* function of *arg* argument" },
		{ "hyperbolic_cosinus",   runtime_->create_method( c.raw(), &HMathematics::hyperbolic_cosinus ),   "( *arg* ) - calculate value of *hyperbolic cosine* function of *arg* argument" },
		{ "hyperbolic_tangens",   runtime_->create_method( c.raw(), &HMathematics::hyperbolic_tangens ),   "( *arg* ) - calculate value of *hyperbolic tangent* function of *arg* argument" },
		{ "hyperbolic_cotangens", runtime_->create_method( c.raw(), &HMathematics::hyperbolic_cotangens ), "( *arg* ) - calculate value of *hyperbolic cotangent* function of *arg* argument" },
		{ "sigmoid",              runtime_->create_method( c.raw(), &HMathematics::sigmoid ),              "( *arg* ) - calculate value of *sigmoid* function of *arg* argument" },
		{ "error_function",       runtime_->create_method( c.raw(), &HMathematics::error_function ),       "( *arg* ) - calculate value of *(Gaussian) error function* of *arg* argument" },
		{ "cumulative_distribution_function",
			runtime_->create_method( c.raw(), &HMathematics::cumulative_distribution_function ),
			"( *arg*, *mu*, *sigma* ) - calculate value of *cumulative distribution function* for normal distribution of *arg* argument, *mu* arithmetic mean and *sigma* standard deviation"
		},
		{ "round",                runtime_->create_method( c.raw(), &HMathematics::round ),                "( *value*[, *precision*] ) - round *value*, potentially to given *precision*" },
		{ "floor",                runtime_->create_method( c.raw(), &HMathematics::floor ),                "( *value* ) - get largest integral value not greater than *value*" },
		{ "ceil",                 runtime_->create_method( c.raw(), &HMathematics::ceil ),                 "( *value* ) - get smallest integral value not less than *value*" },
		{ "differs_at",           runtime_->create_method( c.raw(), &HMathematics::differs_at ),           "( *first*, *second* ) - tell at which decimal position *first* and *second* values have first occurrence of different digit" },
		{ "statistics",           runtime_->create_method( c.raw(), &HMathematics::statistics ),           "( *iterable* ) - create NumberSetStatistics over *iterable* of uniformly types values" }
	};
	c->redefine( nullptr, fd );
	runtime_->huginn()->register_class( c );
	return ( runtime_->object_factory()->create<HMathematics>( c.raw() ) );
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

