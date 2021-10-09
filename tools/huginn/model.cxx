/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "model.hxx"
#include "objectfactory.hxx"
#include "helper.hxx"
#include "time.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace tools {

namespace huginn {

tools::model::HValue huginn_to_cxx( HThread* thread_, HHuginn::value_t const& v_, char const* tag_, int position_ ) {
	M_PROLOG
	tools::model::HValue v;
	switch ( type_tag( v_->type_id() ) ) {
		case ( HHuginn::TYPE::STRING ): {
			v = get_string( v_ );
		} break;
		case ( HHuginn::TYPE::INTEGER ): {
			v = get_integer( v_ );
		} break;
		case ( HHuginn::TYPE::REAL ): {
			v = get_real( v_ );
		} break;
		case ( HHuginn::TYPE::NUMBER ): {
			v = get_number( v_ );
		} break;
		case ( HHuginn::TYPE::CHARACTER ): {
			v = hcore::to_string( get_character( v_ ) );
		} break;
		case ( HHuginn::TYPE::BOOLEAN ): {
			bool booleanValue( get_boolean( v_ ) );
			v = booleanValue ? tools::model::HValue::LITERAL::TRUE : tools::model::HValue::LITERAL::FALSE;
		} break;
		case ( HHuginn::TYPE::NONE ): {
#undef NULL
			v = tools::model::HValue::LITERAL::NULL;
		} break;
		case ( HHuginn::TYPE::TUPLE ): {
			HTuple::values_t const& data( static_cast<HTuple const*>( v_.raw() )->value() );
			for ( HHuginn::value_t const& e : data ) {
				v.push_back( huginn_to_cxx( thread_, e, tag_, position_ ) );
			}
		} break;
		case ( HHuginn::TYPE::LIST ): {
			HList::values_t const& data( static_cast<HList const*>( v_.raw() )->value() );
			for ( HHuginn::value_t const& e : data ) {
				v.push_back( huginn_to_cxx( thread_, e, tag_, position_ ) );
			}
		} break;
		case ( HHuginn::TYPE::LOOKUP ): {
			HLookup::values_t const& data( static_cast<HLookup const*>( v_.raw() )->value() );
			tools::model::HValue::members_t& m( v.get_members() );
			for ( HLookup::values_t::value_type const& e : data ) {
				if ( e.first->type_id() != HHuginn::TYPE::STRING ) {
					throw HHuginn::HHuginnRuntimeException(
						"Keys in "_ys.append( tag_ ).append( " objects must be `string`s, not " ).append( a_type_name( e.first->get_class() ) ).append( "." ),
						thread_->runtime().file_id(),
						position_
					);
				}
				m.insert( make_pair( get_string( e.first ), huginn_to_cxx( thread_, e.second, tag_, position_ ) ) );
			}
		} break;
		case ( HHuginn::TYPE::DEQUE ): {
			HDeque::values_t const& data( static_cast<HDeque const*>( v_.raw() )->value() );
			for ( HHuginn::value_t const& e : data ) {
				v.push_back( huginn_to_cxx( thread_, e, tag_, position_ ) );
			}
		} break;
		case ( HHuginn::TYPE::DICT ): {
			HDict const& dict( *static_cast<HDict const*>( v_.raw() ) );
			HDict::values_t const& data( dict.value() );
			tools::model::HValue::members_t& m( v.get_members() );
			if ( ! data.is_empty() && ( dict.key_type()->type_id() != HHuginn::TYPE::STRING ) ) {
				throw HHuginn::HHuginnRuntimeException(
					"Keys in "_ys.append( tag_ ).append( " objects must be `string`s, not " ).append( a_type_name( dict.key_type() ) ).append( "." ),
					thread_->runtime().file_id(),
					position_
				);
			}
			for ( HLookup::values_t::value_type const& e : data ) {
				m.insert( make_pair( get_string( e.first ), huginn_to_cxx( thread_, e.second, tag_, position_ ) ) );
			}
		} break;
		case ( HHuginn::TYPE::ORDER ): {
			HOrder::values_t const& data( static_cast<HOrder const*>( v_.raw() )->value() );
			for ( HHuginn::value_t const& e : data ) {
				v.push_back( huginn_to_cxx( thread_, e, tag_, position_ ) );
			}
		} break;
		case ( HHuginn::TYPE::SET ): {
			HSet::values_t const& data( static_cast<HSet const*>( v_.raw() )->value() );
			for ( HHuginn::value_t const& e : data ) {
				v.push_back( huginn_to_cxx( thread_, e, tag_, position_ ) );
			}
		} break;
		default: {
			HObjectFactory& of( thread_->object_factory() );
			if ( v_->get_class() == of.time_class() ) {
				v = static_cast<huginn::HTime const*>( v_.raw() )->value();
			} else {
				throw HHuginn::HHuginnRuntimeException(
					"Value of type "_ys.append( a_type_name( v_->get_class() ) ).append( " is not " ).append( tag_ ).append( " serializable." ),
					thread_->runtime().file_id(),
					position_
				);
			}
		}
	}
	return v;
	M_EPILOG
}

HHuginn::value_t cxx_to_huginn( HThread* thread_, tools::model::HValue const& v_, int position_ ) {
	M_PROLOG
	HObjectFactory& of( thread_->object_factory() );
	HHuginn::value_t v( of.none_value() );
	switch ( v_.type() ) {
		case ( tools::model::HValue::TYPE::STRING ): {
			v = of.create_string( v_.get_string() );
		} break;
		case ( tools::model::HValue::TYPE::INTEGER ): {
			v = of.create_integer( v_.get_integer() );
		} break;
		case ( tools::model::HValue::TYPE::REAL ): {
			v = of.create_real( v_.get_real() );
		} break;
		case ( tools::model::HValue::TYPE::NUMBER ): {
			v = of.create_number( v_.get_number() );
		} break;
		case ( tools::model::HValue::TYPE::TIME ): {
			v = of.create<huginn::HTime>( of.time_class(), v_.get_time() );
		} break;
		case ( tools::model::HValue::TYPE::LITERAL ): {
			tools::model::HValue::LITERAL literal( v_.get_literal() );
			if ( literal == tools::model::HValue::LITERAL::TRUE ) {
				v = of.true_value();
			} else if ( literal == tools::model::HValue::LITERAL::FALSE ) {
				v = of.false_value();
			}
		} break;
		case ( tools::model::HValue::TYPE::ARRAY ): {
			HHuginn::values_t data;
			for ( tools::model::HValue const& e : v_.get_elements() ) {
				data.push_back( cxx_to_huginn( thread_, e, position_ ) );
			}
			v = of.create_list( yaal::move( data ) );
		} break;
		case ( tools::model::HValue::TYPE::MAP ): {
			v = of.create_lookup();
			huginn::HLookup* lookup( static_cast<huginn::HLookup*>( v.raw() ) );
			huginn::HLookup::values_t& data( lookup->value() );
			HAnchorGuard<huginn::HLookup> ag( *lookup, thread_, position_ );
			for ( tools::model::HValue::members_t::value_type const& m : v_.get_members() ) {
				data.insert( make_pair( of.create_string( m.first ), cxx_to_huginn( thread_, m.second, position_ ) ) );
			}
		} break;
		case ( tools::model::HValue::TYPE::UNINITIALIZED ): break;
	}
	return v;
	M_EPILOG
}

}

}

}

