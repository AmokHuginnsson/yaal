#include <set>
#include <vector>
#include <list>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <iostream>
#include <fstream>
#include <cerrno>
#include <../include/sys/stat.h>

#ifdef __GNUC__
#include <dirent.h>
#else /* __GNUC__ */
#ifndef __MSVCXX__
#error Unknown build environment.
#endif /* not __MSVCXX__ */
#include <io.h>
#include <direct.h>
#include <windows.h>
#define mkdir( x, y ) _mkdir( ( x ) )
#define sleep( x ) Sleep( ( x ) * 1000 )
#define stat _stat
#define rmdir _rmdir
#define unlink _unlink
#endif

#include "helper.hxx"

using namespace std;
using namespace string_helper;

void process_line( string& );
void process_file( string const&, string const& );
void delete_fucking_old_files_no_matter_fucking_what( string const& );
void create_fucking_new_directory_no_matter_fucking_what( string const& );
void remove_all( string const& );
bool is_directory( string const& );
string dirname( string const& );
string basename( string const& );
string strip_prefix( string, string );
string dirname_basename( string const& );
void copy_file( string const&, string const& );

int main( int argc_, char** argv_ ) {
	int errorCode = 0;
	try {
/*
		if ( argc_ == 2 ) {
			cout << "`" << argv_[1] << "' is considered" << ( is_directory( argv_[1] ) ? "" : " not" ) << " to be a directory" << endl;
			cout << "dirname of `" << argv_[1] << "' is: " << dirname( argv_[1] ) << endl;
			return ( 1 );
		}
		if ( argc_ == 2 ) {
			remove_all( argv_[1] );
			return ( 1 );
		}
*/
		if ( argc_ != 4 )
			throw invalid_argument( "mkheaders: exactly 3 arguments required" );
		string dirRoot( argv_[1] );
		string dirBuild( argv_[2] );
		typedef set<string> paths_t;
		vector<string> paths;
		tokenize( argv_[3], paths, " \t;" );
		set<string> headers;
		transform( paths.begin(), paths.end(), inserter( headers, headers.begin() ), bind2nd( ptr_fun( strip_prefix ), dirRoot + "/" ) );
		if ( ! is_directory( dirRoot ) )
			throw invalid_argument( "mkheaders: DIR_ROOT not a directory" );
		if ( ! is_directory( dirBuild ) )
			throw invalid_argument( "mkheaders: DIR_BUILD not a directory" );
		string dirHeaders( dirBuild + "/yaal" );
		set<string> components;
		transform( paths.begin(), paths.end(), inserter( components, components.begin() ), dirname_basename );
		cout << "Making headers ... " << endl
			<< "dir root: " << dirRoot << endl
			<< "dir build: " << dirBuild << endl
			<< "dir headers: " << dirHeaders << endl
			<< "yaal configuration header path: " << dirHeaders << "/config.hxx" << endl
			<< "Components: \n\t";
		copy( components.begin(), components.end(), ostream_iterator<string>( cout, "\n\t" ) );
		cout << endl;
		delete_fucking_old_files_no_matter_fucking_what( dirHeaders );
		create_fucking_new_directory_no_matter_fucking_what( dirHeaders );
		for ( paths_t::const_iterator it = components.begin(), end = components.end(); it != end; ++ it )
			create_fucking_new_directory_no_matter_fucking_what( dirHeaders + "/" + *it );
		int toProcess( headers.size() );
		cout << "Files to process: " << toProcess << endl;
		for ( paths_t::const_iterator it = headers.begin(), end = headers.end(); it != end; ++ it )
			process_file( dirRoot + "/" + *it, dirHeaders + "/" + *it );
//		copy_file( s._configPath, s._destinationPath / s._configPath.leaf() );
		ofstream o( ( dirHeaders + "/yaal.hxx" ).c_str() );
		static char const D_UNIQ[] = "hstreaminterface";
		ifstream i( ( dirHeaders + "/hcore/" + ( string( D_UNIQ ) + ".hxx" ) ).c_str() );
		string line;
		int lines = 0;
		while ( ! getline( i, line ).fail() && ( lines ++ < 26 ) ) {
			int pos = 0;
			if ( ( pos = line.find( D_UNIQ ) ) != string::npos ) {
				line.erase( pos, sizeof ( D_UNIQ ) - 1 );
				line.insert( pos, "yaal" );
			}
			o << line << endl;
		}
		o << "#ifndef YAAL_YAAL_HXX_INCLUDED" << endl
			<< "#define YAAL_YAAL_HXX_INCLUDED 1" << endl << endl
			<< "#include <yaal/config.hxx>" << endl;
		for ( paths_t::const_iterator it = headers.begin(), end = headers.end(); it != end; ++ it )
			o << "#include <yaal/" << *it << ">" << endl;
		o << endl << "#endif /* not YAAL_YAAL_HXX_INCLUDED */" << endl << endl;
		ifstream ci( ( dirBuild + "/config.hxx" ).c_str() );
		ofstream co( ( dirHeaders + "/config.hxx" ).c_str() );
		static char const PACKAGE_S[] = "define PACKAGE_";
		static char const LIB_INFIX[] = "define LIB_INFIX";
		while ( ! getline( ci, line ).fail() ) {
			int pos = 0;
			if ( ( pos = line.find( PACKAGE_S ) ) != string::npos ) {
				line.erase( pos, sizeof ( PACKAGE_S ) - 1 );
				line.insert( pos, "define YAAL_PACKAGE_" );
			} else if ( line.find( LIB_INFIX ) != string::npos )
				line = "#undef LIB_INFIX";
			co << line << endl;
		}
#ifndef __GNUC__
		copy_file( dirRoot + "/_aux/msvcxx/cleanup.hxx", dirHeaders + "/cleanup.hxx" );
		copy_file( dirRoot + "/_aux/msvcxx/client-fix.hxx", dirHeaders + "/fix.hxx" );
#endif /* not __GNUC__ */
	} catch ( exception const& e ) {
		errorCode = 1;
		cerr << e.what() << endl;
	} catch ( int const& ) {
	}
	return ( errorCode );
}

void copy_file( string const& src_, string const& dst_ ) {
	ifstream i( src_.c_str() );
	ofstream o( dst_.c_str() );
	string line;
	while ( ! getline( i, line ).fail() )
		o << line << endl;
	return;
}

void process_file( string const& from, string const& to ) {
	/* cout << from << " --> " << to << endl; */
	ifstream i( from.c_str() );
	ofstream o( to.c_str() );
	string line;
	while ( ! getline( i, line ).fail() ) {
		process_line( line );
		o << line << endl;
	}
}

void process_line( string& line ) {
	if ( line.find( "#include" ) == 0 ) {
		int posOpen = line.find( "\"" );
		int posClose = ( posOpen != string::npos ) ? line.find( "\"", posOpen + 1 ) : string::npos;
		if ( ( posOpen != string::npos ) && ( posClose != string::npos ) ) {
			line[ posOpen ] = '<';
			line[ posClose ] = '>';
			line.erase( posClose + 1 );
			line.insert( posOpen + 1, "yaal/" );
		}
	}
}

void delete_fucking_old_files_no_matter_fucking_what( string const& p ) {
	bool fail( false );
	while ( is_directory( p ) ) {
		if ( fail ) {
			sleep( 1 );
			cout << "+" << endl;
		}
		remove_all( p );
		fail = true;
	}
}

void create_fucking_new_directory_no_matter_fucking_what( string const& p ) {
	bool fail( false );
	while ( ! is_directory( p ) ) {
		if ( fail ) {
			sleep( 1 );
			cout << "+" << endl;
		}
		mkdir( p.c_str(), 0 );
		fail = true;
	}
}

bool is_directory( string const& p ) {
	struct stat s;
	bool dir( false );
	if ( ! stat( p.c_str(), &s ) )
		dir = ( s.st_mode & S_IFDIR ) ? true : false;
	return ( dir );
}

string dirname( string const& p ) {
	string dir;
/*	clog << "testing path: `" << p << "'" << endl; */
	if ( ! p.empty() ) {
		if ( ! is_directory( p ) ) {
			int long delimIdx( p.find_last_of( "/\\" ) );
			if ( delimIdx != string::npos )
				dir = dirname( string( p, 0, delimIdx ) );
		} else
			dir = p;
	}
	return ( dir );
}

string basename( string const& p ) {
	int long idx( 0 );
	string name( ( ( idx = p.find_last_of( "/\\" ) ) != string::npos ) ? string( p, idx + 1 ) : p );
	return ( name );
}

void remove_all( string const& p ) {
	string toRemove( ( p.find_last_of( "/\\" ) == p.size() - 1 ) ? dirname( p ) : p );
	if ( is_directory( toRemove ) ) {
#ifdef __GNUC__
		DIR* d( opendir( toRemove.c_str() ) );
		if ( ! d )
			throw runtime_error( "cannot open directory: " + toRemove );
#else /* __GNUC__ */
		struct _finddata_t findBuf;
		intptr_t ff( _findfirst( ( toRemove + "/*" ).c_str(), &findBuf ) );
		if ( ff < 0 )
			throw runtime_error( "cannot open directory: " + toRemove );
#endif /* not __GNUC__ */
		try {
#ifdef __GNUC__
			dirent* ent( NULL );
			while ( ( ent = readdir( d ) ) ) {
				if ( ent && ent->d_name ) {
					string name( ent->d_name );
					if ( ( name != "." ) && ( name != ".." ) )
						remove_all( toRemove + "/" + name );
				}
			}
#else /* __GNUC__ */
			do {
				string name( findBuf.name );
				if ( ( name != "." ) && ( name != ".." ) )
					remove_all( toRemove + "/" + name );
			} while ( ! _findnext( ff, &findBuf ) );
#endif /* not __GNUC__ */
		} catch ( ... ) {
#ifdef __GNUC__
			closedir( d );
#else /* __GNUC__ */
			_findclose( ff );
#endif /* not __GNUC__ */
			throw;
		}
#ifdef __GNUC__
		closedir( d );
#else /* __GNUC__ */
		_findclose( ff );
#endif /* not __GNUC__ */
		rmdir( toRemove.c_str() );
	} else
		unlink( toRemove.c_str() );
}

string dirname_basename( string const& p )
	{ return basename( dirname( p ) ); }

string strip_prefix( string item_, string prefix_ ) {
	return ( item_.find( prefix_ ) == 0 ? string( item_, prefix_.size() ) : item_ );
}

