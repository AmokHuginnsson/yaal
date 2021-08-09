#include <algorithm>
#include <string>
#include <exception>
#include <iterator>
#include <iostream>
#include <fstream>
#include <cctype>

using namespace std;

typedef std::pair<std::string::size_type, std::string::size_type> location_t;
location_t locate_project_name( std::string const& str_ ) {
	location_t l( { std::string::npos, std::string::npos } );
	std::string::size_type start( str_.find( '`' ) );
	if ( start != std::string::npos ) {
		++ start;
		std::string::size_type pos( start );
		while ( ( pos != str_.size() ) && std::islower( str_[pos] ) ) {
			++ pos;
		}
		if ( ( pos != str_.size() ) && ( str_[pos] == '`' ) ) {
			l = make_pair( start, pos - start );
		}
	}
	return l;
}

std::string get_project_name( std::istream& src_ ) {
	string name;
	string line;
	while ( std::getline( src_, line ).good() ) {
		location_t l = locate_project_name( line );
		if ( l.first != std::string::npos ) {
			name = line.substr( l.first, l.second );
			break;
		}
	}
	return name;
}

std::string toupper( std::string const& str_ ) {
	string s;
	std::transform( str_.begin(), str_.end(), std::back_insert_iterator<std::string>( s ), static_cast<int(*)(int)>( &std::toupper ) );
	return s;
}

int main( int argc_, char** argv_ ) try {
	if ( argc_ != 3 ) {
		throw runtime_error( "Exactly two parameters requires: mklicense {dest} {src}." );
	}
	ifstream src( argv_[2] );
	if ( ! src ) {
		throw runtime_error( string( "Source license file does not exist: " ).append( argv_[2] ).append( "." ) );
	}
	ofstream out( argv_[1] );
	if ( ! out ) {
		throw runtime_error( string( "Cannot create output file: " ).append( argv_[1] ).append( "." ) );
	}
	string name( toupper( get_project_name( src ) ) );
	src.seekg( 0, std::ios::beg );
	out <<
		"#ifndef " << name << "_LICENSE_HXX_INCLUDED\n"
		"#define " << name << "_LICENSE_HXX_INCLUDED 1\n"
		"\n"
		"char const LICENSE[] =\n";
	string line;
	char const dt[] = "0123456789-";
	while ( getline( src, line ).good() ) {
		location_t l = locate_project_name( line );
		if ( l.first != std::string::npos ) {
			std::string::size_type start( l.first + l.second );
			start = line.find_first_of( dt, start );
			if ( start != std::string::npos ) {
				std::string::size_type pos( line.find_first_not_of( dt, start ) );
				if ( pos != std::string::npos ) {
					out << "\t\""
						<< line.substr( 0, l.first )
						<< "%s"
						<< line.substr( l.first + l.second, start - ( l.first + l.second ) )
						<< "%s"
						<< line.substr( pos )
						<< "\\n\"\n";
					continue;
				}
			}
		}
		out << "\t\"" << line << "\\n\"\n";
	}
	out <<
		";\n"
		"\n"
		"#endif /* #ifndef " << name << "_LICENSE_HXX_INCLUDED */\n" << endl;
	return ( 0 );
} catch ( std::exception const& ex ) {
	cerr << ex.what() << endl;
	return ( 1 );
}

