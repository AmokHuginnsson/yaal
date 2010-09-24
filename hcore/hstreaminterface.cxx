/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstreaminterface.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hstreaminterface.hxx"

#include "hcore/hstring.hxx"

namespace yaal
{

namespace hcore
{

char const* const HStreamInterface::eols = "\r\n"; /* order matters */

HStreamInterface::HStreamInterface( void )
	: _cache( 1, HChunk::STRATEGY::GEOMETRIC ), _offset( 0 ),
	_wordCache(), _fill( ' ' ), _width( 0 ), _base( BASES::DEC )
	{
	return;
	}

HStreamInterface::~HStreamInterface( void )
	{
	return;
	}

HStreamInterface& HStreamInterface::do_output( HString const& string_ )
	{
	M_PROLOG
	if ( ! string_.is_empty() )
		do_write( string_.raw(), string_.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( char const* string_ )
	{
	M_PROLOG
	if ( string_ )
		do_write( string_, static_cast<int long>( ::strlen( string_ ) ) );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( bool bool_ )
	{
	M_PROLOG
	char buf( bool_ ? '1' : '0' );
	do_write( &buf, sizeof ( buf ) );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( char char_ )
	{
	M_PROLOG
	char copy( char_ );
	do_write( &copy, sizeof ( copy ) );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( char unsigned charUnsigned_ )
	{
	M_PROLOG
	char unsigned copy( charUnsigned_ );
	do_write( &copy, sizeof ( copy ) );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( int short shortInteger_ )
	{
	M_PROLOG
	int long tmp = shortInteger_;
	return ( do_output( tmp ) );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( int short unsigned unsignedShortInteger_ )
	{
	M_PROLOG
	int long unsigned tmp = unsignedShortInteger_;
	return ( do_output( tmp ) );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( int integer_ )
	{
	M_PROLOG
	int long tmp = integer_;
	return ( do_output( tmp ) );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( int unsigned unsignedInteger_ )
	{
	M_PROLOG
	int long unsigned tmp = unsignedInteger_;
	return ( do_output( tmp ) );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( int long longInteger_ )
	{
	M_PROLOG
	_wordCache.format( _base == BASES::DEC ? "%ld" : ( _base == BASES::HEX ) ? "%lx" : "%lo", longInteger_ );
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
	}

int long HStreamInterface::reformat( void )
	{
	M_PROLOG
	int long len( _wordCache.get_length() );
	if ( _width )
		{
		if ( _width > len )
			{
			_wordCache.insert( 0, _width - len, static_cast<char>( _fill ) );
			len = _width;
			}
		_width = 0;
		}
	return ( len );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( int long unsigned unsignedLongInteger_ )
	{
	M_PROLOG
	_wordCache.format( _base == BASES::DEC ? "%lu" : ( _base == BASES::HEX ) ? "%lx" : "%lo", unsignedLongInteger_ );
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( double double_ )
	{
	M_PROLOG
	_wordCache.format( "%f", double_ );
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( double long longDouble_ )
	{
	M_PROLOG
	_wordCache.format( "%.12Lf", longDouble_ );
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( float float_ )
	{
	M_PROLOG
	_wordCache.format( "%f", float_ );
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( void const* ptr_ )
	{
	M_PROLOG
	_wordCache.format( "0x%lx", ptr_ );
	do_write( _wordCache.raw(), _wordCache.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( manipulator_t const& HFILE_INTERFACE )
	{
	M_PROLOG
	return ( HFILE_INTERFACE( *this ) );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_output( HManipulator const& manipulator_ )
	{
	M_PROLOG
	manipulator_( *this );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& endl( HStreamInterface& file_ )
	{
	M_PROLOG
	file_.do_write( "\n", 1 );
	file_.do_flush();
	return ( file_ );
	M_EPILOG
	}

HStreamInterface& flush( HStreamInterface& file_ )
	{
	M_PROLOG
	file_.do_flush();
	return ( file_ );
	M_EPILOG
	}

HStreamInterface& dec( HStreamInterface& iface_ )
	{
	M_PROLOG
	return ( iface_.set_base( HStreamInterface::BASES::DEC ) );
	M_EPILOG
	}

HStreamInterface& hex( HStreamInterface& iface_ )
	{
	M_PROLOG
	return ( iface_.set_base( HStreamInterface::BASES::HEX ) );
	M_EPILOG
	}

HStreamInterface& oct( HStreamInterface& iface_ )
	{
	M_PROLOG
	return ( iface_.set_base( HStreamInterface::BASES::OCT ) );
	M_EPILOG
	}

HStreamInterface::HManipulator setw( int width_ )
	{
	M_PROLOG
	return ( HStreamInterface::HManipulator( width_, &HStreamInterface::HManipulator::set_width ) );
	M_EPILOG
	}

HStreamInterface::HManipulator setfill( int fill_ )
	{
	M_PROLOG
	return ( HStreamInterface::HManipulator( fill_, &HStreamInterface::HManipulator::set_fill ) );
	M_EPILOG
	}

int long HStreamInterface::do_read_until( HString& message_,
		char const* const stopSet_, bool stripDelim_ )
	{
	M_PROLOG
	return ( read_until_n( message_, meta::max_signed<int long>::value, stopSet_, stripDelim_ ) );
	M_EPILOG
	}

int long HStreamInterface::do_read_until_n( HString& message_, int long maxCount_,
		char const* const stopSet_, bool stripDelim_ )
	{
	M_PROLOG
	int long nRead = 0; /* how many bytes were read in this single invocation */
	int long iPoolSize = _cache.size();
	char* buffer = _cache.raw(); /* read cache buffer */
	bool byDelim( false );
	bool bySize( false );
	do
		{
		/* Let's check if next read wont exceed size of our read buffer.
		 * 1 byte or terminating zero and one byte for at least one new character,
		 * hence + 2 in condition.
		 */
		if ( ( _offset + 2 ) > iPoolSize )
			{
			_cache.realloc( _offset + 2 );
			buffer = _cache.raw(); /* update read cache buffer ptr, reallocation could move previous buffer into another memomry position */
			iPoolSize = _cache.size();
			}
		/* We read only one byte at a time. */
		nRead = read( buffer + _offset, sizeof ( char ) * 1 );
		/* nRead can be one of the following:
		 * nRead > 0 - a successful read, we shall check for stop char and possibly continue reading.
		 * nRead == 0 - stream is blocking and has just been closed or has no data to read and is internally non-blocking.
		 * nRead < 0 - an error occured, read opration could be externally interrupted.
		 */
		}
	while ( ( nRead > 0 )
			&& ( ! ( byDelim = ( ::strchr( stopSet_, buffer[ _offset ++ ] ) ? true : false ) ) ) /* We increment _offset only if read succeeded. */
			&& ( ! ( bySize = ( _offset >= maxCount_ ) ) ) );
	if ( nRead >= 0 )
		{
		M_ASSERT( _offset >= 0 );
		message_.assign( buffer, _offset - ( ( ( _offset > 0 ) && stripDelim_ && byDelim ) ? 1 : 0 ) );
		nRead = _offset;
		_offset = 0;
		}
	else
		message_.clear();
	return ( nRead );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( HString& word )
	{
	M_PROLOG
	if ( read_word() )
		word = _wordCache;
	return ( *this );
	M_EPILOG
	}

bool HStreamInterface::read_word( void )
	{
	M_PROLOG
	while ( read_until( _wordCache, _whiteSpace_, true ) < 0 )
		;
	return ( _wordCache.get_length() > 0 );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( bool& b )
	{
	M_PROLOG
	if ( read_word() )
		b = lexical_cast<bool>( _wordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( char& c )
	{
	M_PROLOG
	if ( read_word() )
		c = _wordCache[ 0 ];
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( char unsigned& cu )
	{
	M_PROLOG
	if ( read_word() )
		cu = _wordCache[ 0 ];
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( int short& is )
	{
	M_PROLOG
	if ( read_word() )
		is = lexical_cast<int short>( _wordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( int short unsigned& isu )
	{
	M_PROLOG
	if ( read_word() )
		isu = lexical_cast<int short unsigned>( _wordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( int& i )
	{
	M_PROLOG
	if ( read_word() )
		i = lexical_cast<int>( _wordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( int unsigned& iu )
	{
	M_PROLOG
	if ( read_word() )
		iu = lexical_cast<int unsigned>( _wordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( int long& il )
	{
	M_PROLOG
	if ( read_word() )
		il = lexical_cast<int long>( _wordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( int long unsigned& ilu )
	{
	M_PROLOG
	if ( read_word() )
		ilu = lexical_cast<int long unsigned>( _wordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( double& d )
	{
	M_PROLOG
	if ( read_word() )
		d = lexical_cast<double>( _wordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( double long& dl )
	{
	M_PROLOG
	if ( read_word() )
		dl = lexical_cast<double long>( _wordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( float& f )
	{
	M_PROLOG
	if ( read_word() )
		f = lexical_cast<float>( _wordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_input( void const*& )
	{
	M_PROLOG
	return ( *this );
	M_EPILOG
	}


int long HStreamInterface::read( void* const buffer_, int long size_ )
	{
	M_PROLOG
	return ( do_read( buffer_, size_ ) );
	M_EPILOG
	}

int long HStreamInterface::write( void const* const buffer_, int long size_ )
	{
	M_PROLOG
	return ( do_write( buffer_, size_ ) );
	M_EPILOG
	}

bool HStreamInterface::is_valid( void ) const
	{
	M_PROLOG
	return ( do_is_valid() );
	M_EPILOG
	}

void HStreamInterface::flush( void ) const
	{
	M_PROLOG
	do_flush();
	return;
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_set_fill( int fill_ )
	{
	M_PROLOG
	_fill = fill_;
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_set_width( int width_ )
	{
	M_PROLOG
	_width = width_;
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::do_set_base( BASES::enum_t base_ )
	{
	M_PROLOG
	_base = base_;
	return ( *this );
	M_EPILOG
	}

HStreamInterface::HManipulator::HManipulator( int value_, ACTION_t action_ )
	: _value( value_ ), _action( action_ )
	{
	}

void HStreamInterface::HManipulator::operator()( HStreamInterface& iface_ ) const
	{
	M_PROLOG
	(this->*_action)( iface_ );
	return;
	M_EPILOG
	}

void HStreamInterface::HManipulator::set_fill( HStreamInterface& iface_ ) const
	{
	M_PROLOG
	iface_.set_fill( _value );
	return;
	M_EPILOG
	}

void HStreamInterface::HManipulator::set_width( HStreamInterface& iface_ ) const
	{
	M_PROLOG
	iface_.set_width( _value );
	return;
	M_EPILOG
	}

}

}

