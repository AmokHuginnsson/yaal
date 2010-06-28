#include <algorithm>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>

#include "helper.hxx"

using namespace std;
using namespace string_helper;

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
