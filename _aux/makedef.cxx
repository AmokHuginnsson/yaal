#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>

#include "helper.hxx"

#define ASSERT( cond ) do { if ( ! ( cond ) ) { throw logic_error( string( "Assertion failed: `" ) + #cond + "'." ); } } while ( 0 )
#define ENSURE( cond ) do { if ( ! ( cond ) ) { throw runtime_error( string( "Bad input would derail the execution flow: `" ) + #cond + "'." ); } } while ( 0 )

using namespace std;
using namespace string_helper;

typedef map<string, string> dictionary_t;

void handle_options( int argc_, char** argv_, dictionary_t& opts_ ) {
	for ( int i( 1 ); i < argc_; ++ i ) {
		ASSERT( i % 2 ); /* option name */
		string name( argv_[ i ] );
		if ( name.substr( 0, 2 ) != "--" ) {
			throw runtime_error( "Expected option, got: `" + name + "'." );
		}
		dictionary_t::iterator it( opts_.find( name ) );
		if ( it == opts_.end() ) {
			throw runtime_error( "Option: `" + name + "' is not supported." );
		}
		ENSURE( ++ i < argc_ );
		it->second = argv_[i];
	}
	return;
}

std::string remove_address( std::string s_ ) {
	while ( ! s_.empty() ) {
		std::string::size_type hexPos( s_.find( "0x" ) );
		if ( hexPos == std::string::npos ) {
			break;
		}
		std::string::size_type atPos( s_.find( "@", hexPos ) );
		if ( atPos == std::string::npos ) {
			break;
		}
		s_.erase( hexPos, atPos - hexPos );
	}
	return s_;
}

bool compare_symbols( std::string const& left_, std::string const& right_ ) {
	return ( remove_address( left_ ) < remove_address( right_ ) );
}

int main( int argc_, char** argv_ ) {
	int ret( 0 );
	dictionary_t options;
	options.insert( make_pair<string, string>( "--source", "" ) );
	options.insert( make_pair<string, string>( "--destination", "" ) );
	options.insert( make_pair<string, string>( "--append", "" ) );
	options.insert( make_pair<string, string>( "--exclude", "" ) );
	try {
		typedef set<string> word_list_t;
		word_list_t exclude;
		handle_options( argc_, argv_, options );
		dictionary_t::const_iterator excludeOption( options.find( "--exclude" ) );
		ASSERT( excludeOption != options.end() );

		if ( ! excludeOption->second.empty() ) {
			ifstream excludeFile( excludeOption->second.c_str() );
			ENSURE( !! excludeFile );
			string line;
			while ( ! getline( excludeFile, line ).fail() ) {
				exclude.insert( line );
			}
		}
		dictionary_t::const_iterator sourceOption( options.find( "--source" ) );
		ASSERT( sourceOption != options.end() );
		ENSURE( ! sourceOption->second.empty() );
		dictionary_t::const_iterator destinationOption( options.find( "--destination" ) );
		ASSERT( destinationOption != options.end() );
		ENSURE( ! destinationOption->second.empty() );
		dictionary_t::const_iterator appendOption( options.find( "--append" ) );
		ASSERT( appendOption != options.end() );
		ifstream sourceFile( sourceOption->second.c_str() );
		ENSURE( !! sourceFile );
		ofstream destinationFile(
			destinationOption->second.c_str(),
			( appendOption->second == "true" )
				? ios::app
				: ios::trunc
		);
		ENSURE( !! destinationFile );
		string line;
		word_list_t s;
		bool inSymbols( false );
		while ( ! getline( sourceFile, line ).fail() ) {
			if ( line.find( "public symbols" ) != string::npos ) {
				inSymbols = true;
				continue;
			}
			if ( line.find( "  Summary" ) != string::npos ) {
				break;
			}
			if ( inSymbols ) {
				vector<string> t;
				tokenize( line, t );
				if ( ( t.size() > 1 ) && ( t[1].find( "??_C@_" ) != 0 ) ) {
					s.insert( t[1] );
				}
			}
		}
		if ( appendOption->second != "true" ) {
			destinationFile << "EXPORTS" << endl;
		}
		set_difference(
			s.begin(), s.end(),
			exclude.begin(), exclude.end(),
			ostream_iterator<string>( destinationFile, "\n" ),
			compare_symbols
		);
	} catch ( exception const& e ) {
		ret = 1;
		cerr << "Exception caught: " << e.what() << endl;
	}
	return ret;
}

