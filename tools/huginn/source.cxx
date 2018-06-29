/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "source.hxx"
#include "preprocessor.hxx"
#include "tools/streamtools.hxx"
#include "tools/stringalgo.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace huginn {

HSource::HSource( void )
	: _name()
	, _orig()
	, _preprocessed()
	, _skips()
	, _comments()
	, _skippedLines( 0 ) {
	return;
}

void HSource::reset( void ) {
	M_PROLOG
	_skippedLines = 0;
	_comments.clear();
	_skips.clear();
	_preprocessed.clear();
	_orig.clear();
	_name.clear();
	return;
	M_EPILOG
}

void HSource::load( yaal::hcore::HStreamInterface& stream_, yaal::hcore::HString const& name_, int skippedLines_ ) {
	M_PROLOG
	static int const PAGE_SIZE( static_cast<int>( system::get_page_size() ) );
	int nRead( 0 );
	int block( 0 );
	M_ASSERT( _orig.is_empty() );
	HChunk readBuffer;
	int readSize( 0 );
	do {
		readBuffer.realloc( ( block + 1 ) * PAGE_SIZE );
		nRead = static_cast<int>( stream_.read( readBuffer.get<char>() + block * PAGE_SIZE, PAGE_SIZE ) );
		M_ENSURE( nRead >= 0 );
		readSize += nRead;
		++ block;
	} while ( nRead == PAGE_SIZE );
	_name = name_.is_empty() ? get_stream_id( &stream_ ) : name_;
	_orig.assign( readBuffer.get<char>(), readSize );
	_skippedLines = skippedLines_;
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
	if ( ! _orig.is_empty() ) {
		HPrepocessor pp( _orig.begin(), _orig.end() );
		_skips[0] = 0;
		int pos( -1 );
		int skipsTotal( 0 );
		for ( HPrepocessor::HIterator it( pp.begin() ), endIt( pp.end() ); it != endIt; ++ it ) {
			int newPos( static_cast<int>( it.raw() - _orig.begin() ) );
			if ( newPos > ( pos + 1 ) ) {
				skipsTotal += ( newPos - ( pos + 1 ) );
				_skips[static_cast<int>( _preprocessed.get_length() )] = skipsTotal;
				HString comment( pp.comment() );
				typedef HArray<HString> lines_t;
				lines_t lines( string::split<lines_t>( comment, "\n", HTokenizer::SKIP_EMPTY ) );
				for ( HString& l : lines ) {
					l.trim( "/*\t " );
				}
				comment = string::join( lines, " " );
				if ( ! comment.is_empty() ) {
					_comments.insert( make_pair( get_coordinate( newPos ).line() - 1, comment ) );
				}
			}
			pos = newPos;
			_preprocessed.push_back( *it );
		}
	}
	return;
	M_EPILOG
}

int HSource::real_position( int position_ ) const {
	M_PROLOG
	int errorPosition( 0 );
	if ( ! _orig.is_empty() ) {
		skips_t::const_iterator it( _skips.upper_bound( position_ ) );
		-- it;
		M_ENSURE( it != _skips.end() );
		errorPosition = it->second + position_;
	}
	return ( errorPosition );
	M_EPILOG
}

HHuginn::HCoordinate HSource::get_coordinate( int position_ ) const {
	M_PROLOG
	/* +1 because we count lines starting from 1 (not starting from 0) */
	int line( static_cast<int>( count( _orig.begin(), _orig.begin() + position_, NEWLINE ) ) + 1 );
	int lastNewlinePosition( 0 );
	if ( line > 1 ) {
		int nlPos( static_cast<int>( _orig.find_last( NEWLINE, position_ - 1 ) ) );
		++ nlPos;
		lastNewlinePosition = nlPos;
	}
	int column( ( position_ - lastNewlinePosition ) + 1 );
	return ( HHuginn::HCoordinate( line + _skippedLines, column ) );
	M_EPILOG
}

char const* HSource::error_message( int code_ ) const {
	return ( ::error_message( code_ ) );
}

yaal::hcore::HString HSource::get_snippet( int from_, int len_ ) const {
	M_PROLOG
	return ( _preprocessed.substr( from_, len_ ) );
	M_EPILOG
}

yaal::hcore::HString const& HSource::get_comment( int position_ ) const {
	M_PROLOG
	int origPos( real_position( position_ ) );
	HHuginn::HCoordinate coord( get_coordinate( origPos ) );
	comments_t::const_iterator it( _comments.find( coord.line() - 2 ) );
	static HString const noComment;
	return ( it != _comments.end() ? it->second : noComment );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HSource::begin( void ) const {
	M_PROLOG
	return ( _preprocessed.begin() );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HSource::end( void ) const {
	M_PROLOG
	return ( _preprocessed.end() );
	M_EPILOG
}

void HSource::dump_preprocessed( yaal::hcore::HStreamInterface& stream_ ) const {
	M_PROLOG
	stream_<< _preprocessed;
	return;
	M_EPILOG
}

}

}

}

