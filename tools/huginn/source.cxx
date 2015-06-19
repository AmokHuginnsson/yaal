/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  source.cxx - this file is integral part of `yaal' project.

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
#include "source.hxx"
#include "preprocessor.hxx"
#include "tools/streamtools.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HSource::HSource( void )
	: _name(),
	_orig(),
	_origSize( 0 ),
	_preprocessed(),
	_preprocessedSize( 0 ),
	_skips() {
	return;
}

void HSource::load( yaal::hcore::HStreamInterface& stream_ ) {
	M_PROLOG
	static int const PAGE_SIZE( static_cast<int>( system::get_page_size() ) );
	int nRead( 0 );
	int block( 0 );
	M_ASSERT( _origSize == 0 );
	do {
		_orig.realloc( ( block + 1 ) * PAGE_SIZE );
		nRead = static_cast<int>( stream_.read( _orig.get<char>() + block * PAGE_SIZE, PAGE_SIZE ) );
		M_ENSURE( nRead >= 0 );
		_origSize += nRead;
		++ block;
	} while ( nRead == PAGE_SIZE );
	_name = get_stream_id( &stream_ );
	return;
	M_EPILOG
}

yaal::hcore::HString const& HSource::name( void ) const {
	M_PROLOG
	return ( _name );
	M_EPILOG
}

/*
 * Strip C-style comments, concatenate literal strings.
 */
void HSource::preprocess( void ) {
	M_PROLOG
	_preprocessed.realloc( _origSize );
	HPrepocessor pp( _orig.get<char>(), _orig.get<char>() + _origSize );
	char* dst( _preprocessed.get<char>() );
	_skips[0] = 0;
	int pos( -1 );
	int skipsTotal( 0 );
	for ( HPrepocessor::HIterator it( pp.begin() ), endIt( pp.end() ); it != endIt; ++ it, ++ dst ) {
		int newPos( static_cast<int>( it.raw() - _orig.get<char>() ) );
		if ( newPos > ( pos + 1 ) ) {
			skipsTotal += ( newPos - ( pos + 1 ) );
			_skips[static_cast<int>( dst - _preprocessed.get<char>() )] = skipsTotal;
		}
		pos = newPos;
		*dst = *it;
	}
	_preprocessedSize = static_cast<int>( dst - _preprocessed.get<char>() );
	return;
	M_EPILOG
}

int HSource::error_position( int position_ ) const {
	M_PROLOG
	skips_t::const_iterator it( _skips.upper_bound( position_ ) );
	-- it;
	M_ENSURE( it != _skips.end() );
	int errorPosition( it->second + position_ );
	return ( errorPosition );
	M_EPILOG
}

HHuginn::HErrorCoordinate HSource::error_coordinate( int position_ ) const {
	M_PROLOG
	char const* src( _orig.get<char>() );
	/* +1 because we count lines starting from 1 (not starting from 0) */
	int line( static_cast<int>( count( src, src + position_, NEWLINE ) ) + 1 );
	int lastNewlinePosition( 0 );
	if ( line > 1 ) {
		char const* nl( static_cast<char const*>( ::memrchr( src, NEWLINE, static_cast<size_t>( position_ ) ) ) );
		M_ASSERT( nl );
		int nlPos( static_cast<int>( nl - src ) );
		++ nlPos;
		lastNewlinePosition = nlPos;
	}
	int column( ( position_ - lastNewlinePosition ) + 1 );
	return ( HHuginn::HErrorCoordinate( line, column ) );
	M_EPILOG
}

char const* HSource::error_message( int code_ ) const {
	return ( ::error_message( code_ ) );
}

yaal::hcore::HString HSource::get_snippet( int from_, int len_ ) const {
	M_PROLOG
	return ( HString( _preprocessed.get<char>() + from_, len_ ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HSource::begin( void ) const {
	M_PROLOG
	return ( _preprocessed.get<char const>() );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HSource::end( void ) const {
	M_PROLOG
	return ( _preprocessed.get<char const>() + _preprocessedSize );
	M_EPILOG
}

void HSource::dump_preprocessed( yaal::hcore::HStreamInterface& stream_ ) const {
	M_PROLOG
	static int const PAGE_SIZE( static_cast<int>( system::get_page_size() ) );
	int totalWritten( 0 );
	do {
		int nWritten( static_cast<int>( stream_.write( _preprocessed.get<char>() + totalWritten,
						_preprocessedSize - totalWritten >= PAGE_SIZE ? PAGE_SIZE : _preprocessedSize - totalWritten ) ) );
		M_ENSURE( nWritten >= 0 );
		totalWritten += nWritten;
	} while ( totalWritten < _preprocessedSize );
	return;
	M_EPILOG
}

}

}

}

