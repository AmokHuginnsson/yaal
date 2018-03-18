/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "semanticbuffer.hxx"
#include "thread.hxx"
#include "runtime.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::tools::huginn;

namespace yaal {

namespace hcore {
struct HNumber::ElementaryFunctions {
	static void serialize( HSemanticBuffer& buffer_, HNumber const& number_ ) {
		M_PROLOG
		buffer_.write( number_._precision );
		buffer_.write( number_._leafCount );
		buffer_.write( number_._integralPartSize );
		buffer_.write( static_cast<u8_t>( number_._negative ? 1 : 0 ) );
		buffer_.write( number_._canonical.raw(), number_._leafCount * static_cast<int>( sizeof ( HNumber::integer_t ) ) );
		return;
		M_EPILOG
	}
};
}

namespace tools {

namespace huginn {

HSemanticBuffer::HSemanticBuffer( yaal::hcore::HChunk& buffer_, yaal::hcore::HUTF8String& converter_, huginn::HThread* thread_, int position_ )
	: _buffer( buffer_ )
	, _converter( converter_ )
	, _size( 0 )
	, _cycleTracker()
	, _context{ thread_, position_ } {
	return;
}

void HSemanticBuffer::serialize( HHuginn::value_t const& val_ ) {
	M_PROLOG
	_cycleTracker.check(
		val_,
		_context._thread->current_frame()->file_id(),
		_context._position
	);
	yaal::u8_t t( static_cast<u8_t>( val_->type_id().get() ) );
	write( t );
	switch ( static_cast<HHuginn::TYPE>( t ) ) {
		case ( HHuginn::TYPE::NONE ): break;
		case ( HHuginn::TYPE::BOOLEAN ): {
			write( static_cast<i8_t>( static_cast<HHuginn::HBoolean const*>( val_.raw() )->value() ? 1 : 0 ) );
		} break;
		case ( HHuginn::TYPE::INTEGER ): {
			write( static_cast<HHuginn::HInteger const*>( val_.raw() )->value() );
		} break;
		case ( HHuginn::TYPE::REAL ): {
			write( static_cast<HHuginn::HReal const*>( val_.raw() )->value() );
		} break;
		case ( HHuginn::TYPE::CHARACTER ): {
			write( static_cast<HHuginn::HCharacter const*>( val_.raw() )->value().get() );
		} break;
		case ( HHuginn::TYPE::STRING ): {
			write( static_cast<HHuginn::HString const*>( val_.raw() )->value() );
		} break;
		case ( HHuginn::TYPE::NUMBER ): {
			HNumber::ElementaryFunctions::serialize( *this, static_cast<HHuginn::HNumber const*>( val_.raw() )->value() );
		} break;
		case ( HHuginn::TYPE::TUPLE ): {
			HHuginn::HTuple::values_t const& val( static_cast<HHuginn::HTuple const*>( val_.raw() )->value() );
			write( val.get_size() );
			for ( HHuginn::value_t const& v : val ) {
				serialize( v );
			}
		} break;
		case ( HHuginn::TYPE::LIST ): {
			HHuginn::HList::values_t const& val( static_cast<HHuginn::HList const*>( val_.raw() )->value() );
			write( val.get_size() );
			for ( HHuginn::value_t const& v : val ) {
				serialize( v );
			}
		} break;
		case ( HHuginn::TYPE::DEQUE ): {
			HHuginn::HDeque::values_t const& val( static_cast<HHuginn::HDeque const*>( val_.raw() )->value() );
			write( val.get_size() );
			for ( HHuginn::value_t const& v : val ) {
				serialize( v );
			}
		} break;
		case ( HHuginn::TYPE::DICT ): {
			HHuginn::HDict::values_t const& val( static_cast<HHuginn::HDict const*>( val_.raw() )->value() );
			write( val.get_size() );
			for ( HHuginn::HDict::values_t::value_type const& v : val ) {
				serialize( v.first );
				serialize( v.second );
			}
		} break;
		case ( HHuginn::TYPE::LOOKUP ): {
			HHuginn::HLookup::values_t const& val( static_cast<HHuginn::HLookup const*>( val_.raw() )->value() );
			write( val.get_size() );
			for ( HHuginn::HLookup::values_t::value_type const& v : val ) {
				serialize( v.first );
				serialize( v.second );
			}
		} break;
		case ( HHuginn::TYPE::ORDER ): {
			HHuginn::HOrder::values_t const& val( static_cast<HHuginn::HOrder const*>( val_.raw() )->value() );
			write( val.get_size() );
			for ( HHuginn::value_t const& v : val ) {
				serialize( v );
			}
		} break;
		case ( HHuginn::TYPE::SET ): {
			HHuginn::HSet::values_t const& val( static_cast<HHuginn::HSet const*>( val_.raw() )->value() );
			write( val.get_size() );
			for ( HHuginn::value_t const& v : val ) {
				serialize( v );
			}
		} break;
		case ( HHuginn::TYPE::FUNCTION_REFERENCE ): {
			HHuginn::identifier_id_t id(	static_cast<HHuginn::HFunctionReference const*>( val_.raw() )->identifier_id() );
			write( _context._thread->runtime().identifier_name( id ) );
		} break;
		default: {
			throw HHuginn::HHuginnRuntimeException(
				"`"_ys
					.append( val_->get_class()->name() )
					.append( "` is not a serializable type." ),
				_context._thread->current_frame()->file_id(),
				_context._position
			);
		}
	}
	_cycleTracker.done( val_ );
	return;
	M_EPILOG
}

void HSemanticBuffer::write( yaal::hcore::HString const& str_ ) {
	M_PROLOG
	_converter.assign( str_ );
	write( _converter.byte_count() );
	write( _converter.c_str(), _converter.byte_count() );
	return;
	M_EPILOG
}

void HSemanticBuffer::write( void const* data_, int long size_ ) {
	M_PROLOG
	_buffer.realloc( _size + size_ );
	::memcpy( _buffer.get<char>() + _size, data_, static_cast<size_t>( size_ ) );
	_size += size_;
	return;
	M_EPILOG
}

}

}

}

