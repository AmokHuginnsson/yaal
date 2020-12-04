/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/hcore.hxx"
#include "hcore/hsocket.hxx"
#include "hcore/system.hxx"
#include "tools/stringalgo.hxx"
#include "tools/base64.hxx"
#include "tools/hzipstream.hxx"
#include "hcore/unicode.hxx"
#include "hcore/hlog.hxx"

M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "http.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace tools {

namespace http {

enum class STATUS {
	CONTINUE                        = 100,
	OK                              = 200,
	CREATED                         = 201,
	ACCEPTED                        = 202,
	NO_CONTENT                      = 204,
	RESET_CONTENT                   = 205,
	PARTIAL_CONTENT                 = 206,
	MOVED_PERMAMENTLY               = 301,
	FOUND                           = 302,
	SEE_OTHER                       = 303,
	NOT_MODIFIED                    = 304,
	USE_PROXY                       = 305,
	TEMPORARY_REDIRECT              = 307,
	PERMANENT_REDIRECT              = 308,
	BAD_REQUEST                     = 400,
	UNAUTHORIZED                    = 401,
	FORBIDDEN                       = 403,
	NOT_FOUND                       = 404,
	METHOD_NOT_ALLOWED              = 405,
	NOT_ACCEPTABLE                  = 406,
	REQUEST_TIMEOUT                 = 408,
	GONE                            = 410,
	LENGTH_REQUIRED                 = 411,
	PAYLOAD_TOO_LARGE               = 413,
	URI_TOO_LONG                    = 414,
	UNSUPPORTED_MEDIA_TYPE          = 415,
	RANGE_NOT_SATISFIABLE           = 416,
	MISDIRECTED_REQUEST             = 421,
	UPGRADE_REQUIRED                = 426,
	TOO_MANY_REQUESTS               = 429,
	REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
	INTERNAL_SERVER_ERROR           = 500,
	NOT_IMPLEMENTED                 = 501,
	BAD_GATEWAY                     = 502,
	SERVICE_UNAVAILABLE             = 503,
	GATEWAY_TIMEOUT                 = 504,
	HTTP_VERSION_NOT_SUPPORTED      = 505,
	VARIANT_ALSO_NEGOTIATES         = 506,
	NETWORK_AUTHENTICATION_REQUIRED = 511
};

namespace {

struct URL {
	yaal::hcore::HSocket::socket_type_t socketType;
	yaal::hcore::HString host;
	yaal::hcore::HString path;
	int port;
};

class HHTTPStream : public yaal::hcore::HStreamInterface {
public:
	typedef HHTTPStream this_type;
	typedef yaal::hcore::HStreamInterface base_type;
	struct TRANSFER_ENCODING;
	/*! \brief Bits used for specifying the way files are opened.
	 */
	typedef HBitFlag<TRANSFER_ENCODING> transfer_encoding_t;
	struct TRANSFER_ENCODING {
		static M_YAAL_TOOLS_PUBLIC_API transfer_encoding_t const IDENTITY;
		static M_YAAL_TOOLS_PUBLIC_API transfer_encoding_t const CHUNKED;
		static M_YAAL_TOOLS_PUBLIC_API transfer_encoding_t const DEFLATE;
	};
private:
	yaal::hcore::HStreamInterface::ptr_t _socket;
	transfer_encoding_t _transferEncoding;
	int _contentLength;
	int _contentBytesLeft;
	yaal::hcore::HChunk _httpCache;
	int _cachedHTTPBytes;
	int _offsetInChunk;
	yaal::hcore::HString _lineCache;
	int _maxCacheSize;
public:
	HHTTPStream( yaal::hcore::HStreamInterface::ptr_t const& socket_, int contentLength_, transfer_encoding_t transferEncoding_ )
		: _socket( socket_ )
		, _transferEncoding( transferEncoding_ )
		, _contentLength( contentLength_ )
		, _contentBytesLeft( _transferEncoding & TRANSFER_ENCODING::CHUNKED ? 0 : contentLength_ )
		, _httpCache()
		, _cachedHTTPBytes( 0 )
		, _offsetInChunk( 0 )
		, _lineCache()
		, _maxCacheSize( system::get_page_size() ) {
		if ( _transferEncoding & TRANSFER_ENCODING::DEFLATE ) {
			_socket = make_pointer<HZipStream>( _socket, HZipStream::MODE::INFLATE );
		}
		_cache.realloc( _maxCacheSize );
		return;
	}
private:
	virtual int long do_write( void const*, int long ) override {
		throw HHTTPException( "Write on a read only stream" );
	}
	virtual void do_flush( void ) override {
		throw HHTTPException( "Flush on a read only stream" );
	}
	virtual int long do_read( void* data_, int long size_ ) override;
	virtual void do_seek( int long, SEEK ) override {
		throw HHTTPException( "Seek on a read only, forward only stream" );
	}
	virtual bool do_is_valid( void ) const override {
		return ( ( _cachedHTTPBytes > 0 ) || _socket->is_valid() );
	}
	virtual POLL_TYPE do_poll_type( void ) const override {
		return ( _socket->poll_type() );
	}
	virtual void const* do_data( void ) const override {
		return ( _socket->data() );
	}
	virtual int do_immediate_read_size( void ) const override {
		return ( _socket->immediate_read_size() );
	}
	virtual int do_pending_write_size( void ) const override {
		return ( _socket->pending_write_size() );
	}
	HHTTPStream( HHTTPStream const& src ) = delete;
	HHTTPStream& operator = ( HHTTPStream const& src ) = delete;
};
HHTTPStream::transfer_encoding_t const HHTTPStream::TRANSFER_ENCODING::IDENTITY = transfer_encoding_t::new_flag();
HHTTPStream::transfer_encoding_t const HHTTPStream::TRANSFER_ENCODING::CHUNKED = transfer_encoding_t::new_flag();
HHTTPStream::transfer_encoding_t const HHTTPStream::TRANSFER_ENCODING::DEFLATE = transfer_encoding_t::new_flag();

HHTTPStream::transfer_encoding_t parse_transfer_encoding( yaal::hcore::HString const& );
yaal::hcore::HTime parse_time( yaal::hcore::HString const&, yaal::hcore::HString const& );
yaal::hcore::HString parse_content_disposition( yaal::hcore::HString const& );
char const* status_string( STATUS );
URL parse_url( yaal::hcore::HString const& );
STATUS get_response_status( yaal::hcore::HString const& );
yaal::hcore::HString make_request_string( URL const&, HRequest const& );
void push_payload( yaal::hcore::HStreamInterface&, yaal::hcore::HStreamInterface& );

}

HResponse call( HRequest const& request_ ) {
	HString request( request_.url() );
	HChunk devNull;
	HString host;
	int port( 0 );
	HStreamInterface::ptr_t stream;
	HTime lastModified( now_local() );
	HString contentType;
	int contentLength( 0 );
	HString filename;
	while ( ! _isKilled_ ) {
		URL url( parse_url( request ) );
		if ( ( url.port != port ) || ( url.host != host ) ) {
			stream = make_pointer<HSocket>( url.socketType );
		}
		HSocket* sock( static_cast<HSocket*>( stream.raw() ) );
		sock->connect( url.host, url.port );
		*stream << make_request_string( url, request_ ) << flush;
		if ( request_.payload() ) {
			push_payload( *stream, *request_.payload() );
		}
		HString line;
		HString headerName;
		HString headerValue;
		if ( ! getline( *stream, line ).good() ) {
			throw HHTTPException( "Failed to receive a response header." );
		}
		HString responseHeader( line );
		STATUS status( get_response_status( responseHeader ) );
		HHTTPStream::transfer_encoding_t transferEncoding( HHTTPStream::TRANSFER_ENCODING::IDENTITY );
		HString location;
		while ( getline( *stream, line ).good() ) {
			if ( line.is_empty() ) {
				break;
			}
			int long colonPos( line.find( unicode::CODE_POINT::COLON ) );
			if ( colonPos == HString::npos ) {
				throw HHTTPException( "Malformed header received: `"_ys.append( line ).append( "`" ) );
			}
			headerName.assign( line, 0, colonPos );
			headerValue.assign( line, colonPos + 1 );
			headerValue.trim();
			headerName.lower();
			if ( headerName == "transfer-encoding" ) {
				transferEncoding = parse_transfer_encoding( headerValue );
			} else if ( headerName == "content-length" ) {
				contentLength = lexical_cast<int>( headerValue );
			} else if ( headerName == "content-type" ) {
				contentType = headerValue;
			} else if ( headerName == "location" ) {
				location = headerValue;
			} else if ( headerName == "last-modified" ) {
				lastModified = parse_time( headerValue, "http::call("_ys.append( request_.url() ).append( ") - " ).append( headerName ).append( ": " ) );
			} else if ( headerName == "content-disposition" ) {
				filename = parse_content_disposition( headerValue );
#ifdef __DEBUG__
			} else {
				hcore::log( LOG_LEVEL::DEBUG ) << headerName << ": " << headerValue << endl;
#endif
			}
		}
		if ( ( status != STATUS::OK ) && ( contentLength > 0 ) ) {
			devNull.realloc( contentLength );
			stream->read( devNull.raw(), contentLength );
		}
		if ( ! location.is_empty() ) {
			request = location;
			continue;
		}
		if ( status != STATUS::OK ) {
			throw HHTTPException( "Failed to retrieve a resource ("_ys.append( status_string( status ) ).append( "): `" ).append( request ).append( "`" ) );
		}
		stream = make_pointer<HHTTPStream>( stream, contentLength, transferEncoding );
		break;
	}
	return ( HResponse( stream, lastModified, contentType, contentLength, filename ) );
}

namespace {

int long HHTTPStream::do_read( void* data_, int long size_ ) {
	int nRead( 0 );
	while ( size_ > 0 ) {
		if ( _cachedHTTPBytes == 0 ) {
			if ( _contentBytesLeft == 0 ) {
				if ( _transferEncoding & TRANSFER_ENCODING::CHUNKED ) {
					if ( ! getline( *_socket, _lineCache ).good() ) {
						return ( nRead );
					}
					if ( _lineCache.is_empty() ) {
						return ( nRead );
					}
					_contentBytesLeft = stoi( _lineCache, nullptr, 16 );
				} else if ( _contentLength == 0 ) {
					_contentBytesLeft = _maxCacheSize;
				}
			}
			if ( _contentBytesLeft > 0 ) {
				int toCache( min( _contentBytesLeft, _maxCacheSize ) );
				_httpCache.realloc( toCache );
				_cachedHTTPBytes = static_cast<int>( _socket->read( _httpCache.raw(), toCache ) );
				_contentBytesLeft -= _cachedHTTPBytes;
				_offsetInChunk = 0;
				if ( ( _contentBytesLeft == 0 ) && ( _transferEncoding & TRANSFER_ENCODING::CHUNKED ) ) {
					char waste[2];
					_socket->read( waste, 2 );
				}
			} else {
				return ( nRead );
			}
		}
		if ( _cachedHTTPBytes == 0 ) {
			return ( nRead );
		}
		int toRead( min( _cachedHTTPBytes, safe_int::cast<int>( size_ ) ) );
		::memcpy( static_cast<char*>( data_ ) + nRead, _httpCache.get<char>() + _offsetInChunk, static_cast<size_t>( toRead ) );
		size_ -= toRead;
		_cachedHTTPBytes -= toRead;
		_offsetInChunk += toRead;
		nRead += toRead;
	}
	return ( nRead );
}

namespace {
char const* http_type_to_str( HTTP type_ ) {
	switch ( type_ ) {
		case ( HTTP::GET ):    return ( "GET" );
		case ( HTTP::POST ):   return ( "POST" );
		case ( HTTP::PUT ):    return ( "PUT" );
		case ( HTTP::DELETE ): return ( "DELETE" );
		case ( HTTP::PATCH ):  return ( "PATCH" );
		case ( HTTP::HEAD ):   return ( "HEAD" );
	}
	return ( nullptr );
}
}

yaal::hcore::HString make_request_string( URL const& url_, HRequest const& request_ ) {
	HString requestString;
	requestString
		.assign( http_type_to_str( request_.type() ) ).append( " " ).append( url_.path )
		.append(
			" HTTP/1.1\r\n"
			"Host: "
		).append( url_.host ).append( unicode::CODE_POINT::COLON ).append( url_.port )
		.append(
			"\r\n"
			"User-Agent: "
		);
	if ( request_.user_agent().is_empty() ) {
		requestString.append( "codestation-yaal-" ).append( yaal_version( true ) );
	} else {
		requestString.append( request_.user_agent() );
	}
	requestString.append(
		"\r\n"
		"Accept-Charset: utf-8\r\n"
		"Accept-Encoding: identity, deflate\r\n"
	);
	if ( request_.payload() ) {
		requestString.append(
			"Transfer-Encoding: chunked\r\n"
			"Content-Type: "
		);
		if ( ! request_.mime_type().is_empty() ) {
			requestString.append( request_.mime_type() );
		} else {
			requestString.append( "application/octet-stream" );
		}
		requestString.append( "\r\n" );
	}
	for ( HRequest::HHeader const& header : request_.headers() ) {
		requestString.append( header.name() ).append( ": " ).append( header.value() ).append( "\r\n" );
	}
	if ( ! ( request_.login().is_empty() && request_.password().is_empty() ) ) {
		HString auth;
		auth.assign( request_.login() ).append( unicode::CODE_POINT::COLON ).append( request_.password() );
		requestString.append( "Authorization: Basic " ).append( base64::encode( auth ) ).append( "\r\n" );
	}
	requestString.append( "\r\n" );
	return ( requestString );
}

void push_payload( yaal::hcore::HStreamInterface& socket_, yaal::hcore::HStreamInterface& payload_ ) {
	M_PROLOG
	static int const CHUNK_SIZE( system::get_page_size() );
	HChunk chunk( CHUNK_SIZE );
	char* buf( chunk.get<char>() );
	bool ok( true );
	HStreamInterface::BASE base( socket_.get_base() );
	socket_.set_base( HStreamInterface::BASE::HEX );
	while ( ok ) {
		int nRead( static_cast<int>( payload_.read( buf, CHUNK_SIZE ) ) );
		if ( nRead <= 0 ) {
			break;
		}
		socket_.write( nRead ).write( "\r\n", 2 );
		int toWrite( nRead );
		char* p( buf );
		while ( toWrite > 0 ) {
			int nWritten( static_cast<int>( socket_.write( p, toWrite ) ) );
			if ( nWritten <= 0 ) {
				ok = false;
				break;
			}
			toWrite -= nWritten;
			p += nWritten;
		}
		socket_.write( "\r\n", 2 );
	}
	socket_.write( "0\r\n\r\n", 5 );
	socket_.flush();
	socket_.set_base( base );
	return;
	M_EPILOG
}

STATUS get_response_status( yaal::hcore::HString const& responseHeader_ ) {
	M_PROLOG
	// HTTP/1.1 000 Message
	static int long const MINIMUM_RESPONSE_LENGTH( 12 );
	static int const MINOR_VERSION_POS( 7 );
	if (
		( responseHeader_.get_length() < MINIMUM_RESPONSE_LENGTH )
		|| ( responseHeader_.compare( 0, MINOR_VERSION_POS, "HTTP/1." ) != 0 )
		|| (
			( responseHeader_[MINOR_VERSION_POS] != '0'_ycp )
			&& ( responseHeader_[MINOR_VERSION_POS] != '1'_ycp )
		)
		|| ( responseHeader_[MINOR_VERSION_POS + 1] != ' '_ycp )
	) {
		throw HHTTPException( "Malformed response header: `"_ys.append( responseHeader_ ).append( "`" ) );
	}
	static int const CODE_POS( 9 );
	static int const CODE_LEN( 3 );
	int statusRaw( 0 );
	try {
		statusRaw = lexical_cast<int>( responseHeader_.substr( CODE_POS, CODE_LEN ) );
	} catch ( HLexicalCastException const& e ) {
		throw HHTTPException( "Malformed HTTP status code: `"_ys.append( responseHeader_ ).append( "` - " ).append( e.what() ) );
	}
	STATUS status( static_cast<STATUS>( statusRaw ) );
	if ( ! status_string( status ) ) {
		throw HHTTPException( "Unknown HTTP status code: `"_ys.append( responseHeader_ ).append( "`" ) );
	}
	return ( status );
	M_EPILOG
}

URL parse_url( yaal::hcore::HString const& url_ ) {
	M_PROLOG
	char const HTTP_SCHEME[] = "http://";
	int const HTTP_SCHEME_LEN( static_cast<int>( sizeof ( HTTP_SCHEME ) ) - 1 );
	char const HTTPS_SCHEME[] = "https://";
	int const HTTPS_SCHEME_LEN( static_cast<int>( sizeof ( HTTPS_SCHEME ) ) - 1 );
	bool isHTTP( ( url_.get_length() > HTTP_SCHEME_LEN ) && ( url_.compare( 0, HTTP_SCHEME_LEN, HTTP_SCHEME ) == 0 ) );
	bool isHTTPS( ( url_.get_length() > HTTPS_SCHEME_LEN ) && ( url_.compare( 0, HTTPS_SCHEME_LEN, HTTPS_SCHEME ) == 0 ) );
	if ( ! ( isHTTP || isHTTPS ) ) {
		throw HHTTPException( "Not a valid HTTP(S) URL: "_ys.append( url_ ) );
	}
	int long slashPos( url_.find( unicode::CODE_POINT::SLASH ) );
	M_ASSERT( slashPos != HString::npos );
	HString url( url_.substr( slashPos ) );
	url.trim_left( unicode::CODE_POINT::SLASH );
	slashPos = url.find( unicode::CODE_POINT::SLASH );
	int long colonPos( url.find( unicode::CODE_POINT::COLON ) );
	int long hostEnd( url.get_length() );
	int port( isHTTPS ? 443 : 80 );
	if ( slashPos != HString::npos ) {
		hostEnd = slashPos;
	}
	int long pathStart( hostEnd );
	if (
		( colonPos != HString::npos )
		&& ( ( slashPos == HString::npos ) || ( colonPos < slashPos ) )
	) {
		hostEnd = colonPos;
		int long portStart( colonPos + 1 );
		slashPos = url.find( unicode::CODE_POINT::SLASH, portStart );
		pathStart = slashPos != HString::npos ? slashPos : url.get_length();
		try {
			port = lexical_cast<int>( url.substr( portStart, pathStart - portStart ) );
		} catch ( HLexicalCastException const& e ) {
			throw HHTTPException( "Invalid port number: "_ys.append( e.what() ) );
		}
	}
	HSocket::socket_type_t socketType( HSocket::TYPE::CLIENT | HSocket::TYPE::BLOCKING | HSocket::TYPE::NETWORK );
	if ( isHTTPS ) {
		socketType |= HSocket::TYPE::SSL;
	}
	return (
		URL{
			socketType,
			url.substr( 0, hostEnd ),
			pathStart < url.get_length() ? url.substr( pathStart ) : "/",
			port
		}
	);
	M_EPILOG
}

HHTTPStream::transfer_encoding_t parse_transfer_encoding( yaal::hcore::HString const& value_ ) {
	M_PROLOG
	HHTTPStream::transfer_encoding_t transferEncoding( HHTTPStream::TRANSFER_ENCODING::IDENTITY );
	string::tokens_t methods( string::split( value_, "," ) );
	for ( yaal::hcore::HString& method : methods ) {
		method.trim().lower();
		if ( method == "identity" ) {
			transferEncoding = HHTTPStream::TRANSFER_ENCODING::IDENTITY;
			break;
		} else if ( method == "chunked" ) {
			transferEncoding |= HHTTPStream::TRANSFER_ENCODING::CHUNKED;
		} else if ( method == "deflate" ) {
			transferEncoding |= HHTTPStream::TRANSFER_ENCODING::DEFLATE;
		} else {
			hcore::log( LOG_LEVEL::DEBUG ) << "unsupported transfer encoding: " << method << endl;
		}
	}
	return ( transferEncoding );
	M_EPILOG
}

yaal::hcore::HTime parse_time( yaal::hcore::HString const& value_, yaal::hcore::HString const& context_ ) {
	M_PROLOG
	HTime time( now_local() );
	try {
		time.set_format( _rfc7231DateTimeFormat_ );
		time.from_string( value_ );
	} catch ( HException const& e ) {
		hcore::log( LOG_LEVEL::DEBUG ) << context_ << value_ << " - " << e.what() << endl;
	}
	time.set_format( _iso8601DateTimeFormat_ );
	return ( time );
	M_EPILOG
}

yaal::hcore::HString parse_content_disposition( yaal::hcore::HString const& value_ ) {
	M_PROLOG
	HString filename;
	int long sepPos( value_.find( unicode::CODE_POINT::SEMICOLON ) );
	if ( sepPos == HString::npos ) {
		return ( HString() );
	}
	filename.assign( value_, sepPos + 1 );
	filename.trim();
	char const VALUE_NAME[] = "filename=";
	int const VALUE_NAME_SIZE( static_cast<int>( sizeof ( VALUE_NAME ) ) - 1 );
	if ( ( filename.get_length() <= VALUE_NAME_SIZE ) || ( filename.compare( 0, VALUE_NAME_SIZE, VALUE_NAME ) != 0 ) ) {
		return ( HString() );
	}
	filename.shift_left( VALUE_NAME_SIZE );
	filename.trim();
	filename.trim( "\"" );
	return ( filename );
	M_EPILOG
}

char const* status_string( STATUS status_ ) {
	char const* statusString( nullptr );
	switch ( status_ ) {
		case ( STATUS::CONTINUE ):                        statusString = "Continue";                        break;
		case ( STATUS::OK ):                              statusString = "OK";                              break;
		case ( STATUS::CREATED ):                         statusString = "Created";                         break;
		case ( STATUS::ACCEPTED ):                        statusString = "Accepted";                        break;
		case ( STATUS::NO_CONTENT ):                      statusString = "No Content";                      break;
		case ( STATUS::RESET_CONTENT ):                   statusString = "Reset Content";                   break;
		case ( STATUS::PARTIAL_CONTENT ):                 statusString = "Partial Content";                 break;
		case ( STATUS::MOVED_PERMAMENTLY ):               statusString = "Moved Permamently";               break;
		case ( STATUS::FOUND ):                           statusString = "Found";                           break;
		case ( STATUS::SEE_OTHER ):                       statusString = "See Other";                       break;
		case ( STATUS::NOT_MODIFIED ):                    statusString = "Not Modified";                    break;
		case ( STATUS::USE_PROXY ):                       statusString = "Use Proxy";                       break;
		case ( STATUS::TEMPORARY_REDIRECT ):              statusString = "Temporary Redirect";              break;
		case ( STATUS::PERMANENT_REDIRECT ):              statusString = "Permanent Redirect";              break;
		case ( STATUS::BAD_REQUEST ):                     statusString = "Bad Request";                     break;
		case ( STATUS::UNAUTHORIZED ):                    statusString = "Unauthorized";                    break;
		case ( STATUS::FORBIDDEN ):                       statusString = "Forbidden";                       break;
		case ( STATUS::NOT_FOUND ):                       statusString = "Not Found";                       break;
		case ( STATUS::METHOD_NOT_ALLOWED ):              statusString = "Method Not Allowed";              break;
		case ( STATUS::NOT_ACCEPTABLE ):                  statusString = "Not Acceptable";                  break;
		case ( STATUS::REQUEST_TIMEOUT ):                 statusString = "Request Timeout";                 break;
		case ( STATUS::GONE ):                            statusString = "Gone";                            break;
		case ( STATUS::LENGTH_REQUIRED ):                 statusString = "Length Required";                 break;
		case ( STATUS::PAYLOAD_TOO_LARGE ):               statusString = "Payload Too Large";               break;
		case ( STATUS::URI_TOO_LONG ):                    statusString = "URI Too Long";                    break;
		case ( STATUS::UNSUPPORTED_MEDIA_TYPE ):          statusString = "Unsupported Media Type";          break;
		case ( STATUS::RANGE_NOT_SATISFIABLE ):           statusString = "Range Not Satisfiable";           break;
		case ( STATUS::MISDIRECTED_REQUEST ):             statusString = "Misdirected Request";             break;
		case ( STATUS::UPGRADE_REQUIRED ):                statusString = "Upgrade Required";                break;
		case ( STATUS::TOO_MANY_REQUESTS ):               statusString = "Too Many Requests";               break;
		case ( STATUS::REQUEST_HEADER_FIELDS_TOO_LARGE ): statusString = "Request Header Fields Too Large"; break;
		case ( STATUS::INTERNAL_SERVER_ERROR ):           statusString = "Internal Server Error";           break;
		case ( STATUS::NOT_IMPLEMENTED ):                 statusString = "Not Implemented";                 break;
		case ( STATUS::BAD_GATEWAY ):                     statusString = "Bad Gateway";                     break;
		case ( STATUS::SERVICE_UNAVAILABLE ):             statusString = "Service Unavailable";             break;
		case ( STATUS::GATEWAY_TIMEOUT ):                 statusString = "Gateway Timeout";                 break;
		case ( STATUS::HTTP_VERSION_NOT_SUPPORTED ):      statusString = "HTTP Version Not Supported";      break;
		case ( STATUS::VARIANT_ALSO_NEGOTIATES ):         statusString = "Variant Also Negotiates";         break;
		case ( STATUS::NETWORK_AUTHENTICATION_REQUIRED ): statusString = "Network Authentication Required"; break;
	}
	return ( statusString );
}

}

}

}

}

