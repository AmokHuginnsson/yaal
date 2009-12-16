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

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hstreaminterface.hxx"

#include "hcore/hstring.hxx"

namespace yaal
{

namespace hcore
{

char const* const HStreamInterface::eols = "\r\n"; /* order matters */

HStreamInterface::HStreamInterface( void )
	: f_oCache( 1, cache_t::AUTO_GROW ), f_iOffset( 0 ), f_sStatus(), f_oWordCache()
	{
	return;
	}

HStreamInterface::~HStreamInterface( void )
	{
	return;
	}

HStreamInterface& HStreamInterface::operator << ( HString const& a_oString )
	{
	M_PROLOG
	if ( ! a_oString.is_empty() )
		do_write( a_oString.raw(), a_oString.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( char const* const& a_pcString )
	{
	M_PROLOG
	if ( a_pcString )
		do_write( a_pcString, ::strlen( a_pcString ) );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( char const& a_cChar )
	{
	M_PROLOG
	char copy( a_cChar );
	do_write( &copy, sizeof ( copy ) );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( int short const& a_iShortInteger )
	{
	M_PROLOG
	int long l_lTmp = a_iShortInteger;
	return ( operator << ( l_lTmp ) );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( int short unsigned const& a_uiUnsignedShortInteger )
	{
	M_PROLOG
	int long unsigned l_ulTmp = a_uiUnsignedShortInteger;
	return ( operator << ( l_ulTmp ) );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( int const& a_iInteger )
	{
	M_PROLOG
	int long l_lTmp = a_iInteger;
	return ( operator << ( l_lTmp ) );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( int unsigned const& a_uiUnsignedInteger )
	{
	M_PROLOG
	int long unsigned l_ulTmp = a_uiUnsignedInteger;
	return ( operator << ( l_ulTmp ) );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( int long const& a_lLongInteger )
	{
	M_PROLOG
	f_oWordCache.format( "%ld", a_lLongInteger );
	do_write( f_oWordCache.raw(), f_oWordCache.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( int long unsigned const& a_ulUnsignedLongInteger )
	{
	M_PROLOG
	f_oWordCache.format( "%lu", a_ulUnsignedLongInteger );
	do_write( f_oWordCache.raw(), f_oWordCache.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( double const& a_dDouble )
	{
	M_PROLOG
	f_oWordCache.format( "%f", a_dDouble );
	do_write( f_oWordCache.raw(), f_oWordCache.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( double long const& a_dLongDouble )
	{
	M_PROLOG
	f_oWordCache.format( "%.12Lf", a_dLongDouble );
	do_write( f_oWordCache.raw(), f_oWordCache.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( float const& a_dFloat )
	{
	M_PROLOG
	f_oWordCache.format( "%f", a_dFloat );
	do_write( f_oWordCache.raw(), f_oWordCache.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( void const* const& a_pvPtr )
	{
	M_PROLOG
	f_oWordCache.format( "0x%lx", a_pvPtr );
	do_write( f_oWordCache.raw(), f_oWordCache.get_length() );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator << ( HStreamInterface& ( *const HFILE_INTERFACE )( HStreamInterface& ) )
	{
	M_PROLOG
	return ( HFILE_INTERFACE( *this ) );
	M_EPILOG
	}

HStreamInterface& endl( HStreamInterface& a_roFile )
	{
	M_PROLOG
	a_roFile.do_write( "\n", 1 );
	return ( a_roFile );
	M_EPILOG
	}

HStreamInterface& flush( HStreamInterface& a_roFile )
	{
	M_PROLOG
	a_roFile.do_flush();
	return ( a_roFile );
	M_EPILOG
	}

HStreamInterface::STATUS const& HStreamInterface::read_until( HString& a_roMessage,
		char const* const a_pcStopSet, bool a_bStripDelim )
	{
	M_PROLOG
	int long nRead = 0; /* how many bytes were read in this single invocation */
	int long iPoolSize = f_oCache.size();
	char* l_pcBuffer = f_oCache.raw(); /* read cache buffer */
	do
		{
		/* Let's check if next read wont exceed size of our read buffer.
		 * 1 byte or terminating zero and one byte for at least one new character,
		 * hence + 2 in condition.
		 */
		if ( ( f_iOffset + 2 ) > iPoolSize )
			{
			f_oCache.pool_realloc( f_iOffset + 2 );
			l_pcBuffer = f_oCache.raw(); /* update read cache buffer ptr, reallocation could move previous buffer into another memomry position */
			iPoolSize = f_oCache.size();
			}
		/* We read only one byte at a time. */
		nRead = read( l_pcBuffer + f_iOffset, sizeof ( char ) * 1 );
		/* nRead can be one of the following:
		 * nRead > 0 - a successful read, we shall check for stop char and possibly continue reading.
		 * nRead == 0 - stream is blocking and has just been closed or has no data to read and is internally non-blocking.
		 * nRead < 0 - an error occured, read opration could be externally interrupted.
		 */
		}
	while ( ( nRead > 0 ) && ! ::strchr( a_pcStopSet, l_pcBuffer[ f_iOffset ++ ] ) );
	if ( nRead >= 0 )
		{
		/* Stripping delimiting character means that read is one byte shorter,
		 * we will zero-terminate later.
		 */
		if ( a_bStripDelim )
			-- f_iOffset;
		if ( f_iOffset >= 0 )
			{
			/* We can zero-terminate and update client buffer.
			 */
			l_pcBuffer[ f_iOffset ] = 0;
			a_roMessage = l_pcBuffer;
			}
		/* This read_until cycle has been completed,
		 * next read_until call will start a new cycle.
		 */
		f_sStatus.octets = f_iOffset;
		f_iOffset = 0;
		if ( ! nRead ) /* Nothing was read, not even a stop character, this means trouble, I mean error. */
			f_sStatus.code = STATUS::ERROR;
		else if ( ( f_sStatus.octets == 0 ) && a_bStripDelim ) /* only stop character has been found in the stream */
			f_sStatus.code = STATUS::REPEAT;
		else /* We have got some meaningful bytes. ayea! */
			f_sStatus.code = STATUS::OK;
		}
	else
		f_sStatus.code = STATUS::REPEAT;
	return ( f_sStatus );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( HString& word )
	{
	M_PROLOG
	if ( read_word() )
		word = f_oWordCache;
	return ( *this );
	M_EPILOG
	}

bool HStreamInterface::read_word( void )
	{
	M_PROLOG
	while ( read_until( f_oWordCache, n_pcWhiteSpace, true ).code == STATUS::REPEAT )
		;
	return ( f_oWordCache.get_length() > 0 );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( char& c )
	{
	M_PROLOG
	if ( read_word() )
		c = f_oWordCache[ 0 ];
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( int short& is )
	{
	M_PROLOG
	if ( read_word() )
		is = lexical_cast<int short>( f_oWordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( int short unsigned& isu )
	{
	M_PROLOG
	if ( read_word() )
		isu = lexical_cast<int short unsigned>( f_oWordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( int& i )
	{
	M_PROLOG
	if ( read_word() )
		i = lexical_cast<int>( f_oWordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( int unsigned& iu )
	{
	M_PROLOG
	if ( read_word() )
		iu = lexical_cast<int unsigned>( f_oWordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( int long& il )
	{
	M_PROLOG
	if ( read_word() )
		il = lexical_cast<int long>( f_oWordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( int long unsigned& ilu )
	{
	M_PROLOG
	if ( read_word() )
		ilu = lexical_cast<int long unsigned>( f_oWordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( double& d )
	{
	M_PROLOG
	if ( read_word() )
		d = lexical_cast<double>( f_oWordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( double long& dl )
	{
	M_PROLOG
	if ( read_word() )
		dl = lexical_cast<double long>( f_oWordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( float& f )
	{
	M_PROLOG
	if ( read_word() )
		f = lexical_cast<float>( f_oWordCache );
	return ( *this );
	M_EPILOG
	}

HStreamInterface& HStreamInterface::operator >> ( void const*& )
	{
	M_PROLOG
	return ( *this );
	M_EPILOG
	}


int long HStreamInterface::read( void* const a_pvBuffer, int long const& a_lSize )
	{
	M_PROLOG
	return ( do_read( a_pvBuffer, a_lSize ) );
	M_EPILOG
	}

int long HStreamInterface::write( void const* const a_pvBuffer, int long const& a_lSize )
	{
	M_PROLOG
	return ( do_write( a_pvBuffer, a_lSize ) );
	M_EPILOG
	}

bool HStreamInterface::is_valid( void ) const
	{
	M_PROLOG
	return ( do_is_valid() );
	M_EPILOG
	}

}

}

