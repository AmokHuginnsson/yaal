/******************************************************************
 *
 *  configure.js
 *
 *  Script file to setup and run cmake to create VS 2008 solution
 *  and projects for the yaal library.
 *
 ******************************************************************/

var globalMessageBuffer = "";
var fs = null;
var dirRoot = null;
var withUt = false;
var ForReading = 1;
var ForWriting = 2;
var ForAppending = 8;
var FAST = 0;

function checkKey( key ) {
	var has = false;
	var value;
	try {
		var regEdit = WScript.createObject( "WScript.Shell" );
		value = regEdit.regRead( key );
		has = true;
	} catch ( ex ) {
	}
	return ( { exists : has, value : value } );
}

function boostInfo( install_path ) {
	var keyPrefix = [ "HKEY_LOCAL_MACHINE\\SOFTWARE\\boostpro.com\\", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Boost-Consulting.com\\", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\boostpro.com\\", "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Boost-Consulting.com\\" ];
	this.installPath = "";
	function versionInfo() {
		this.TAG = "";
		this.vc = "";
		this.thread = "";
		this.target = "";
		this.lib = "";
	}   
	this.version = new versionInfo();
	this.exists = false;
	var fs = new ActiveXObject( "Scripting.FileSystemObject" );
	var versions = [ "1.41", "1.40", "1.39", "1.38.0", "1.37.0", "1.36.0", "1.35.0", "1.34.1", "1.34.0", "1.33.1" ];
	for ( var idx in versions ) {
		if ( this.exists )
			break;
		for ( var k in keyPrefix ) {
			var testPath = keyPrefix[k] + versions[ idx ] + "\\InstallRoot";
			var boostKey = checkKey( testPath );
			if ( boostKey.exists || ( install_path != null ) ) {
				if ( install_path != null )
					this.installPath = install_path;
				else
					this.installPath = boostKey.value;
				this.version.TAG = versions[ idx ];
				var dir = fs.GetFolder( this.installPath + "/lib" );
				var fl = new Enumerator( dir.files );
				var TAG = "boost_program_options";
				var foundFirstOfAKind = false;
				var names = new Array();
				for ( ; ! fl.atEnd(); fl.moveNext() )
					names.push( fl.item().name );
				names.sort();
				for ( var i in names ) {
					var index = 0;
					if ( ( index = names[ i ].indexOf( TAG ) ) >= 0 ) {
						foundFirstOfAKind = true;
						var name = names[ i ].substr( index + TAG.length + 1 );
						var verInfo = name.substr( 0, name.length - 4 ).split( "-", 4 );
						this.version.vc = verInfo[ 0 ];
						this.version.thread = verInfo[ 1 ];
						this.version.target = verInfo[ 2 ];
						if ( this.version.target == "gd" ) {
							this.version.lib = verInfo[ 3 ];
							this.exists = true;
							if ( install_path != null ) {
								this.version.TAG = this.version.lib.replace( '_', '.' );
							}
							break;
						}
					} else if ( foundFirstOfAKind )
						break;
				}
				break;
			}
		}
	}
}

function vcVersion() {
	var versions = [
		[ "Wow6432Node\\", "9.0", "Visual Studio 9 2008" ],
		[ "", "9.0", "Visual Studio 9 2008" ],
		[ "Wow6432Node\\", "8.0", "Visual Studio 8 2005" ],
		[ "", "8.0", "Visual Studio 8 2005" ]
	];
	var version = "";
	var keyPrefix = "HKEY_LOCAL_MACHINE\\SOFTWARE\\";
	var keyMiddle = "Microsoft\\VisualStudio\\";
	for ( var idx in versions ) {
		var testPath = keyPrefix + versions[ idx ][ 0 ] + keyMiddle + versions[ idx ][ 1 ] + "\\Setup\\VS\\MSMDir";
		var vcKey = checkKey( testPath );
		if ( vcKey.exists ) {
			version = versions[ idx ][ 2 ];
			break;
		}
	}
	return ( version );
}

function makeBoostDesc( boostInfo ) {
	var msg = "Boost install path: " + boostInfo.installPath + "\n"
		+ "Boost version: " + boostInfo.version.TAG + "\n"
		+ "Boost Visual Studio version: " + boostInfo.version.vc + "\n"
		+ "Boost Threading: " + boostInfo.version.thread + "\n"
		+ "Boost Target: " + boostInfo.version.target + "\n"
		+ "Boost Library binary version: " + boostInfo.version.lib;
	return ( msg );
}

function msg( str ) {
	if ( str != null ) {
		while ( ( str.length > 0 ) && ( ( str.charAt( str.length - 1 ) == '\n' ) || ( str.charAt( str.length - 1 ) == '\r' ) ) )
			str = str.substr( 0, str.length - 1 );
		if ( ! FAST ) {
			globalMessageBuffer += ( str + "\n" );
		} else if ( str.length > 0 ) {
			WScript.echo( str );
		}
	}
}

function terminate( code ) {
	if ( globalMessageBuffer.charAt( globalMessageBuffer.length - 1 ) == "\n" )
		WScript.echo( globalMessageBuffer.substr( 0, globalMessageBuffer.length - 1 ) );
	else
		WScript.echo( globalMessageBuffer );
	WScript.quit( code );
}

if ( ! Array.prototype.filter ) {
	Array.prototype.filter = function( fun/*, thisp*/ ) {
		var len = this.length;
		if ( typeof fun != "function" )
			throw new TypeError();
		var res = new Array();
		var thisp = arguments[1];
		for ( var i = 0; i < len; ++ i ) {
			if ( i in this ) {
				var val = this[i]; // in case fun mutates this
				if ( fun.call( thisp, val, i, this ) )
					res.push( val );
			}
		}
		return res;
	};
}

try {
	fs = new ActiveXObject( "Scripting.FileSystemObject" );
	dirRoot = fs.GetFolder(".").path.replace( /\\/gm, "/" );

	if ( ! fs.FileExists( dirRoot + "/configure.ac" ) ) {
		WScript.echo( "This script must be runned from project main directory." );
		WScript.quit( 1 );
	}

} catch ( ex ) {
	WScript.echo( ex.description );
	WScript.quit( 1 );
}

try {

	var args = WScript.Arguments;
	var CMAKELISTS_PATH = "";
	var BOOST_INSTALL_PATH = null;
	var BOOST_VERSION = null;
	var EXTRA_INCLUDE_PATH = "";
	var EXTRA_LIBRARY_PATH = "";
	var PREFIX = "";
	var SILENT = 0;
	var VERBOSE = 0;
	var VISUAL_STUDIO_VERSION = vcVersion();

	try {
		var conf = fs.openTextFile( "local.js", ForReading );
		eval( conf.readAll() );
	} catch ( e ) {
		msg( "Reading configuration: " + e.description );
	}

	for ( var i = 0; i < args.count(); ++ i ) {
		var parts = args.Item(i).split('=');
		switch ( parts[0].toUpperCase() ) {
			case "CMAKELISTS_PATH":
				CMAKELISTS_PATH = parts[1];
			break;
			case "EXTRA_INCLUDE_PATH":
				EXTRA_INCLUDE_PATH = parts[1];
			break;
			case "EXTRA_LIBRARY_PATH":
				EXTRA_LIBRARY_PATH = parts[1];
			break;
			case "BOOST_INSTALL_PATH":
				BOOST_INSTALL_PATH = parts[1];
			break;
			case "VISUAL_STUDIO_VERSION":
				VISUAL_STUDIO_VERSION = parts[1];
			break;
			case "PREFIX":
				PREFIX = parts[1];
			break;
			case "FAST":
				FAST = 1;
			break;
			case "VERBOSE":
				VERBOSE = 1;
			break;
			case "SILENT":
				SILENT = 1;
			break;
		}
	}

	var boostInfo = new boostInfo( BOOST_INSTALL_PATH );

	if ( ! boostInfo.exists ) {
		msg( "Cannot locate boost installation (havent you used BoostPro?)!" );
		terminate( 1 );
	}
	if ( ( VISUAL_STUDIO_VERSION == null ) || ( VISUAL_STUDIO_VERSION == "" ) ) {
		msg( "Cannot determine your VisualStudio installation information." );
		terminate( 1 );
	}
	BOOST_VERSION = boostInfo.version.vc
		+ "-" + boostInfo.version.thread
		+ "-" + boostInfo.version.target
		+ "-" + boostInfo.version.lib;
	var eol = dirRoot.charAt( dirRoot.length - 1 );
	if ( ( eol != "\\" ) && ( eol != "/" ) )
		dirRoot += "/";

	eol = boostInfo.installPath.charAt( boostInfo.installPath.length - 1 );
	if ( ( eol != "\\" ) && ( eol != "/" ) )
		boostInfo.installPath += "/";
	boostInfo.installPath = boostInfo.installPath.replace( /\\/gm, "/" );

	msg( "Project root: " + dirRoot );
	msg( "CMake generator: " + VISUAL_STUDIO_VERSION );
	msg( "CMakeLists.txt path: " + CMAKELISTS_PATH );
	msg( makeBoostDesc( boostInfo ) );

	var shell = WScript.createObject( "WScript.Shell" );
	var cmdline = "cmake -G \"" + VISUAL_STUDIO_VERSION + "\" ";
	if ( VERBOSE )
		cmdline += "-DVERBOSE=1 ";
	if ( EXTRA_INCLUDE_PATH != null )
		cmdline += ( "-DCMAKE_INCLUDE_PATH=" + EXTRA_INCLUDE_PATH + " " );
	if ( EXTRA_LIBRARY_PATH != null )
		cmdline += ( "-DCMAKE_LIBRARY_PATH=" + EXTRA_LIBRARY_PATH + " " );
	if ( BOOST_INSTALL_PATH != null )
		cmdline += ( "-DBOOST_INSTALL_PATH=" + BOOST_INSTALL_PATH + " " );
	cmdline += CMAKELISTS_PATH;
	envSys = shell.environment( "System" );
	envProc = shell.environment( "Process" );
	envUser = shell.environment( "User" );
	envProc( "PATH" ) = ( envSys( "PATH" ) + ";" + envProc( "PATH" ) ).split( ";", 1000 ).filter( ( function( obj ){ return ( ! String( obj ).match( "cygwin|unix" ) ); } ) ).join( ";" );
	if ( PREFIX.length > 0 )
		envProc( "PREFIX" ) = PREFIX;
	envProc( "CXX" ) = "";
	envProc( "CC" ) = "";
	envProc.remove( "CXX" );
	envProc.remove( "CC" );
	cmd = shell.exec( cmdline );
	var eoo = true;
	var eoe = true;
	var errBuf;
	var outBuf;
	while ( ( ! ( eoo = cmd.stdout.AtEndOfStream ) ) || ( ! ( eoe = cmd.stderr.AtEndOfStream ) ) ) {
		if ( ! eoo ) {
			var out = cmd.stdout.readLine();
			if ( FAST )
				msg( out );
			else
				outBuf += ( out + "\n" );
		}
		if ( ! eoe ) {
			var err = cmd.stderr.readLine();
			if ( FAST )
				msg( err );
			else
				errBuf += ( err + "\n" );
		}
	}
	if ( ! FAST ) {
		msg( outBuf );
		msg( errBuf );
	}
	if ( ! SILENT ) {
		msg( "Done!" );
		terminate( 0 );
	}
} catch ( e ) {
	msg( "Error: " + e.description );
	terminate( -1 );
}

