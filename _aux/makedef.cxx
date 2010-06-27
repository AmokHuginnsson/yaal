#include <algorithm>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>

using namespace std;

namespace string_helper
{

template<typename T>
void add_element( set<T>& s_, T const& e_ )
	{ s_.insert( e_ ); }

template<typename T>
void add_element( vector<T>& s_, T const& e_ )
	{ s_.push_back( e_ ); }

template<typename T>
void tokenize( std::string const& str, T& tokens, std::string const& delimiters = " \t")
	{
	/* Skip delimiters at beginning. */
	std::string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
	/* Find first "non-delimiter". */
	std::string::size_type pos     = str.find_first_of( delimiters, lastPos );

	while ( ( std::string::npos != pos ) || ( std::string::npos != lastPos ) )
		{
		/* Found a token, add it to the vector. */
		add_element( tokens, str.substr( lastPos, pos - lastPos ) );
		/* Skip delimiters.  Note the "not_of" */
		lastPos = str.find_first_not_of( delimiters, pos );
		/* Find next "non-delimiter" */
		pos = str.find_first_of( delimiters, lastPos );
		}
	}

}

using namespace string_helper;

template<typename to_t, typename from_t>
to_t lexical_cast( from_t const& from )
	{
	std::stringstream ss;
	ss << from;
	to_t to;
	ss >> to;
	return ( to );
	}

int main( int argc_, char** argv_ )
	{
	bool sane( true );
	if ( argc_ != 4 )
		{
		cout << "makedef: this program requires exactly three arguments" << endl;
		sane = false;
		}
	if ( sane )
		{
		ifstream in( argv_[1] );
		if ( ! in )
			{
			cout << "makedef: given symbol file `" << argv_[1] << "' does not exists" << endl;
			sane = false;
			}
		ofstream out( argv_[2] );
		if ( ! in )
			{
			cout << "makedef: cannot create `" << argv_[2] << "' .DEF file" << endl;
			sane = false;
			}
		if ( sane )
			{
			set<string> exclude;
			tokenize( argv_[3], exclude, "," );
			string line;
			bool inSymbols( false );
			typedef set<string> symbols_t;
			symbols_t s;
			while ( ! getline( in, line ).fail() )
				{
				if ( line.find( "public symbols" ) != string::npos )
					{
					inSymbols = true;
					continue;
					}
				if ( line.find( "  Summary" ) != string::npos )
					break;
				if ( inSymbols )
					{
					vector<string> t;
					tokenize( line, t );
					if ( t.size() > 1 )
						s.insert( t[1] );
					}
				}
			out << "EXPORTS" << endl;
			set_difference( s.begin(), s.end(),
				exclude.begin(), exclude.end(),
				ostream_iterator<string>( out, "\n" ) );
			}
		}
	return ( sane ? 0 : 1 );
	}
