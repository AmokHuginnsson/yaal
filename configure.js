/******************************************************************
 *
 *  configure.js
 *
 *  Script file to setup and run cmake to create VS 2013 solution
 *  and projects for the yaal library.
 *
 ******************************************************************/

var shell = WScript.createObject( "WScript.Shell" );
var fs = new ActiveXObject( "Scripting.FileSystemObject" );
var globalMessageBuffer = "";
var dirRoot = null;
var withUt = false;
var ForReading = 1;
var ForWriting = 2;
var ForAppending = 8;
var FAST = -1;

String.prototype.trim = function() {
	return ( this.replace( /^\s+|\s+$/g, '' ) );
}

String.prototype.endsWith = function( target ) {
	return ( this.slice( - target.length ) == target );
}

function checkKey( key ) {
	var has = false;
	var value;
	try {
		value = shell.regRead( key );
		has = true;
	} catch ( ex ) {
	}
	return ( { exists : has, value : value } );
}

function isBoostUsed( buildDefinitionPath ) {
	var used = false;
	try {
		var bd = fs.openTextFile( "./" + buildDefinitionPath + "/CMakeLists.txt", ForReading );
		while ( ! bd.AtEndOfStream ) {
			var line = bd.readLine();
			if ( line.toLowerCase().indexOf( "boost" ) != -1 ) {
				used = true;
				break;
			}
		}
	} catch ( e ) {
		msg( "Reading build definition: " + e.description );
	}
	return used;
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
	var versions = [ "1.43", "1.42", "1.41", "1.40", "1.39", "1.38.0", "1.37.0", "1.36.0", "1.35.0" ];
	for ( var idx in versions ) {
		if ( this.exists ) {
			break;
		}
		for ( var k in keyPrefix ) {
			var testPath = keyPrefix[k] + versions[ idx ] + "\\InstallRoot";
			var boostKey = checkKey( testPath );
			if ( boostKey.exists || ( install_path != null ) ) {
				if ( install_path != null ) {
					this.installPath = install_path;
				} else {
					this.installPath = boostKey.value;
				}
				this.version.TAG = versions[ idx ];
				var dir = fs.GetFolder( this.installPath + "/lib" );
				var fl = new Enumerator( dir.files );
				var TAG = "boost_program_options";
				var foundFirstOfAKind = false;
				var names = new Array();
				for ( ; ! fl.atEnd(); fl.moveNext() ) {
					names.push( fl.item().name );
				}
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
					} else if ( foundFirstOfAKind ) {
						break;
					}
				}
				break;
			}
		}
	}
}

function vcVersion() {
	try {
		var vswhere = "c:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe";
		var cmd = shell.exec( vswhere + " -latest -property catalog_productName" );
		var version = cmd.stdout.readAll().trim().concat( " " );
		var cmd = shell.exec( vswhere + " -latest -property installationVersion" );
		version += cmd.stdout.readAll().split( "." )[0];
		return version;
	} catch ( ex ) {
	}
	var versions = [
		[ "Wow6432Node\\", "14.0", "Visual Studio 14" ],
		[ "", "14.0", "Visual Studio 14" ]
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
	return version;
}

function makeBoostDesc( boostInfo ) {
	var msg = "Boost install path: " + boostInfo.installPath + "\n"
		+ "Boost version: " + boostInfo.version.TAG + "\n"
		+ "Boost Visual Studio version: " + boostInfo.version.vc + "\n"
		+ "Boost Threading: " + boostInfo.version.thread + "\n"
		+ "Boost Target: " + boostInfo.version.target + "\n"
		+ "Boost Library binary version: " + boostInfo.version.lib;
	return msg;
}

function msg( str ) {
	if ( str != null ) {
		while ( ( str.length > 0 ) && ( ( str.charAt( str.length - 1 ) == '\n' ) || ( str.charAt( str.length - 1 ) == '\r' ) ) ) {
			str = str.substr( 0, str.length - 1 );
		}
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
				if ( fun.call( thisp, val, i, this ) ) {
					res.push( val );
				}
			}
		}
		return res;
	};
}

try {

	var args = WScript.Arguments;
	var PROJECT_ROOT = "";
	var BOOST_INSTALL_PATH = null;
	var BOOST_VERSION = null;
	var EXTRA_INCLUDE_PATH = "";
	var EXTRA_LIBRARY_PATH = "";
	var PREFIX = "";
	var SYSCONFDIR = "";
	var LOCALSTATEDIR = "";
	var DATADIR = "";
	var LIBDIR = "";
	var BUILD_TYPE = "debug";
	var BUILD_PACKAGE = 0;
	var YAAL_AUTO_SANITY = 0;
	var SILENT = 0;
	var VERBOSE = 0;
	var VISUAL_STUDIO_VERSION = vcVersion();

	for ( var i = 0; i < args.count(); ++ i ) {
		var parts = args.Item(i).split('=');
		if ( parts[0].toUpperCase() == "PROJECT_ROOT" ) {
			PROJECT_ROOT = parts[1];
		}
	}

	try {
		if ( PROJECT_ROOT == "" ) {
			dirRoot = fs.GetFolder(".").path.replace( /\\/gm, "/" );
		} else {
			dirRoot = PROJECT_ROOT;
		}

		if ( ! fs.FileExists( dirRoot + "/configure.ac" ) ) {
			WScript.echo( "This script must be runned on project main directory, not: `" + dirRoot + "'." );
			WScript.quit( 1 );
		}
	} catch ( ex ) {
		WScript.echo( ex.description );
		WScript.quit( 1 );
	}

	try {
		var conf = fs.openTextFile( dirRoot + "/local.js", ForReading );
		eval( conf.readAll() );
	} catch ( e ) {
		msg( "Reading configuration: " + e.description );
	}

	for ( var i = 0; i < args.count(); ++ i ) {
		var parts = args.Item(i).split('=');
		switch ( parts[0].toUpperCase() ) {
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
			case "SYSCONFDIR":
				SYSCONFDIR = parts[1];
			break;
			case "LOCALSTATEDIR":
				LOCALSTATEDIR = parts[1];
			break;
			case "DATADIR":
				DATADIR = parts[1];
			break;
			case "LIBDIR":
				LIBDIR = parts[1];
			break;
			case "BUILD_TYPE":
				BUILD_TYPE = parts[1];
			break;
			case "YAAL_AUTO_SANITY":
				YAAL_AUTO_SANITY = 1;
			break;
			case "BUILD_PACKAGE":
				BUILD_PACKAGE = 1;
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

	if ( SYSCONFDIR.length == 0 ) {
		SYSCONFDIR = PREFIX + "/etc";
	}

	if ( LOCALSTATEDIR.length == 0 ) {
		LOCALSTATEDIR = PREFIX + "/var";
	}

	if ( DATADIR.length == 0 ) {
		DATADIR = PREFIX + "/share";
	}
	if ( LIBDIR.length == 0 ) {
		LIBDIR = PREFIX + "/lib";
	}

	if ( FAST == -1 ) {
		if ( WScript.FullName.substr( WScript.FullName.length - 11 ).toLowerCase() == "cscript.exe" ) {
			FAST = 1;
		} else {
			FAST = 0;
		}
	}

	if ( ( VISUAL_STUDIO_VERSION == null ) || ( VISUAL_STUDIO_VERSION == "" ) ) {
		msg( "Cannot determine your VisualStudio installation information." );
		terminate( 1 );
	}
	var eol = dirRoot.charAt( dirRoot.length - 1 );
	if ( ( eol != "\\" ) && ( eol != "/" ) ) {
		dirRoot += "/";
	}

	msg( "Project root: " + dirRoot );
	msg( "CMake generator: " + VISUAL_STUDIO_VERSION );
	msg( "CMakeLists.txt path: " + PROJECT_ROOT );

	if ( isBoostUsed( PROJECT_ROOT ) ) {
		var boostInfo = new boostInfo( BOOST_INSTALL_PATH );

		if ( ! boostInfo.exists ) {
			msg( "Cannot locate boost installation (havent you used BoostPro?)!" );
		} else {
			BOOST_VERSION = boostInfo.version.vc
				+ "-" + boostInfo.version.thread
				+ "-" + boostInfo.version.target
				+ "-" + boostInfo.version.lib;

			eol = boostInfo.installPath.charAt( boostInfo.installPath.length - 1 );
			if ( ( eol != "\\" ) && ( eol != "/" ) ) {
				boostInfo.installPath += "/";
			}
			boostInfo.installPath = boostInfo.installPath.replace( /\\/gm, "/" );

			msg( makeBoostDesc( boostInfo ) );
		}
	}

	var cmdline = "cmake -G \"" + VISUAL_STUDIO_VERSION + "\"";
	if ( VISUAL_STUDIO_VERSION.endsWith( " 16" ) ) {
		cmdline += " -T host=x86 -A Win32";
	} else if ( VISUAL_STUDIO_VERSION.endsWith( " 15" ) ) {
		cmdline += " Win32";
	}
	cmdline += " -DCMAKE_BUILD_TYPE=" + BUILD_TYPE;
	if ( BUILD_PACKAGE ) {
		cmdline += " -DBUILD_PACKAGE=1";
	}
	if ( YAAL_AUTO_SANITY ) {
		cmdline += " -DYAAL_AUTO_SANITY=1";
	}
	if ( VERBOSE ) {
		cmdline += " -DVERBOSE=1";
	}
	if ( EXTRA_INCLUDE_PATH != null ) {
		cmdline += ( " -DCMAKE_INCLUDE_PATH=" + EXTRA_INCLUDE_PATH );
	}
	if ( EXTRA_LIBRARY_PATH != null ) {
		cmdline += ( " -DCMAKE_LIBRARY_PATH=" + EXTRA_LIBRARY_PATH );
	}
	if ( BOOST_INSTALL_PATH != null ) {
		cmdline += ( " -DBOOST_INSTALL_PATH=" + BOOST_INSTALL_PATH );
	}
	cmdline += ( " " + PROJECT_ROOT );
	envSys = shell.environment( "System" );
	envProc = shell.environment( "Process" );
	envUser = shell.environment( "User" );
	envProc( "PATH" ) = ( envSys( "PATH" ) + ";" + envProc( "PATH" ) ).split( ";", 1000 ).filter( ( function( obj ){ return ( ! String( obj ).match( "cygwin|unix" ) ); } ) ).join( ";" );
	if ( PREFIX.length > 0 ) {
		envProc( "PREFIX" ) = PREFIX;
	}
	envProc( "SYSCONFDIR" ) = SYSCONFDIR;
	envProc( "LOCALSTATEDIR" ) = LOCALSTATEDIR;
	envProc( "DATADIR" ) = DATADIR;
	envProc( "LIBDIR" ) = LIBDIR;
	envProc( "CXX" ) = "";
	envProc( "CC" ) = "";
	envProc.remove( "CXX" );
	envProc.remove( "CC" );
	msg( cmdline );
	cmd = shell.exec( cmdline );
	var eoo = true;
	var eoe = true;
	var errBuf;
	var outBuf;
	while ( ( ! ( eoo = cmd.stdout.AtEndOfStream ) ) || ( ! ( eoe = cmd.stderr.AtEndOfStream ) ) ) {
		if ( ! eoo ) {
			var out = cmd.stdout.readLine();
			if ( FAST ) {
				msg( out );
			} else {
				outBuf += ( out + "\n" );
			}
		}
		if ( ! eoe ) {
			var err = cmd.stderr.readLine();
			if ( FAST ) {
				msg( err );
			} else {
				errBuf += ( err + "\n" );
			}
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

