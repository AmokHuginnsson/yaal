/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "crypto.hxx"
#include "hdes.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace crypto {

void crypt_3des( yaal::hcore::HStreamInterface::ptr_t src_, yaal::hcore::HStreamInterface::ptr_t dst_, HString const& key_ ) {
	M_PROLOG
	crypt_3des( *src_, *dst_, key_ );
	return;
	M_EPILOG
}

void crypt_3des( yaal::hcore::HStreamInterface& src_, yaal::hcore::HStreamInterface& dst_, HString const& key_ ) {
	M_PROLOG
	static int const BUF_SIZE( 128 );
	HChunk buf( BUF_SIZE );
	HDes des( key_ );
	int long const toRead( buf.get_size() );
	M_ASSERT( toRead == BUF_SIZE );
	char gap( 0 );
	int long nRead( 0 );
	while ( ( nRead = src_.read( buf.raw(), toRead ) ) > 0 ) {
		gap = static_cast<char>( nRead % 8 );
		if ( gap ) {
			gap = static_cast<char>( 8 - gap );
			generate_n( buf.get<char>() + nRead, gap, random::rng_helper::make_random_number_generator( 255 ) );
		}
		int long toWrite( nRead + gap );
		M_ASSERT( toWrite <= toRead );
		des.crypt( buf.get<u8_t>(), toWrite, HDes::CRYPT );
		dst_.write( buf.raw(), toWrite );
	}
	dst_.write( &gap, 1 );
	return;
	M_EPILOG
}

void decrypt_3des( yaal::hcore::HStreamInterface::ptr_t src_, yaal::hcore::HStreamInterface::ptr_t dst_, yaal::hcore::HString const& key_ ) {
	M_PROLOG
	decrypt_3des( *src_, *dst_, key_ );
	return;
	M_EPILOG
}

void decrypt_3des( yaal::hcore::HStreamInterface& src_, yaal::hcore::HStreamInterface& dst_, yaal::hcore::HString const& key_ ) {
	M_PROLOG
	static int const BUF_SIZE( 128 );
	HChunk bufA( BUF_SIZE );
	HChunk bufB( BUF_SIZE );
	HDes des( key_ );
	int long const toRead( bufA.get_size() );
	M_ASSERT( toRead == BUF_SIZE );
	M_ASSERT( bufB.get_size() == BUF_SIZE );
	char signed gap( 0 );
	int long nRead( 0 );
	int long toWrite( 0 );
	char* readBuf( bufA.get<char>() );
	char* writeBuf( bufB.get<char>() );
	while ( ( ( nRead = src_.read( readBuf, toRead ) ) > 0 ) || ( toWrite > 0 ) ) {
		/* There are 3 cases for non-malformed encrypted data:
		 * 1 - we read only full blocks
		 * 2 - we read only gap
		 * 3 - we read both data (some full blocks) and the gap
		 */
		if ( nRead > 0 ) {
			gap = static_cast<char signed>( nRead % 8 );
			if ( gap ) {
				M_ENSURE( gap == 1, "malformed encrypted packet" );
				-- nRead;
				gap = static_cast<char signed>( readBuf[ nRead ] );
				M_ENSURE( ( gap >= 0 ) && ( gap < 8 ) && ( toWrite > 0 ? ( gap <= toWrite ) : ( gap <= nRead ) ), "malformed gap" );
			}
			toWrite -= ( nRead > 0 ? 0 /* full blocks from previous read */ : gap /* only gap has been read */ );
			if ( toWrite > 0 )
				dst_.write( writeBuf, toWrite );
			toWrite = nRead;
			M_ASSERT( ! ( toWrite % 8 ) );
			if ( toWrite > 0 )
				des.crypt( reinterpret_cast<u8_t*>( readBuf ), toWrite, HDes::DECRYPT );
			toWrite -= gap;
			swap( readBuf, writeBuf );
		} else if ( toWrite > 0 ) {
			dst_.write( writeBuf, toWrite );
			toWrite = 0;
		}
	}
	return;
	M_EPILOG
}

}

}

}

