#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <cstdio>
#include <cstring>

#ifdef __MSVCXX__
#include <direct.h>
#define popen _popen
#define getcwd _getcwd
#else
#include <unistd.h>
#endif

using namespace std;

string read_cmd( string const& cmd_ ) {
	static int const BUF_SIZE( 1024 );
	char buf[BUF_SIZE];
	cout << "Executing command: `" << cmd_ << "' in `" << getcwd( buf, BUF_SIZE - 1 ) << "'." << endl;
	FILE* idCmd( popen( cmd_.c_str(), "r" ) );
	string out;
	if ( !! idCmd ) {
		::memset( buf, 0, BUF_SIZE );
		::fread( buf, BUF_SIZE - 1, sizeof ( char ), idCmd );
		out = buf;
		while ( ! out.empty() && ( ( out.back() == '\n' ) || ( out.back() == '\r' ) ) ) {
			out.pop_back();
		}
		::fclose( idCmd );
	}
	return out;
}

int main( int argc_, char** argv_ ) try {
	char env[][16] = { "HOME=", "HOMEPATH=", "USERPROFILE=" };
	for ( char* e : env ) {
		putenv( e );
	}
	if ( argc_ != 3 ) {
		throw runtime_error( string( argv_[0] ).append( " requires exactly 2 arguments!" ) );
	}
	string masterPath( argv_[1] );
	masterPath.append( "/.git/refs/heads/master" );
	if ( ! ifstream( masterPath ) ) {
		throw runtime_error( string( "Bad DIR_ROOT: `" ).append( argv_[1] ).append( "'!" ) );
	}
	string id( read_cmd( "git rev-parse HEAD" ) );
	if ( id.empty() ) {
		ifstream head( masterPath );
		if ( ! head ) {
			throw runtime_error( string( "Cannot open id information file: " ).append( masterPath ) );
		}
		getline( head, id );
	}
	system( "git update-index -q --refresh" );
	int status( system( "git diff-index --quiet HEAD --" ) );
	if ( status ) {
		id.push_back( '*' );
	}
	ifstream orig( argv_[2] );
	string origData;
	getline( orig, origData );
	orig.close();
	string newData( "#define COMMIT_ID \"" );
	newData.append( id ).append( "\"" );
	if ( newData != origData ) {
		ofstream out( argv_[2] );
		if ( ! out ) {
			throw runtime_error( string( "Cannot create: " ).append( argv_[2] ) );
		}
		out << newData << endl;
	}
	return ( 0 );
} catch ( std::exception const& e ) {
	cerr << e.what() << endl;
	return ( 1 );
}

