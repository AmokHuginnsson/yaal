/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file http.hxx
 * \brief Declaration of a `http` namespace.
 */

#ifndef YAAL_TOOLS_HTTP_HXX_INCLUDED
#define YAAL_TOOLS_HTTP_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"
#include "hcore/htime.hxx"
#include "hcore/harray.hxx"

namespace yaal {

namespace tools {

namespace http {

enum class HTTP {
	GET,
	POST,
	PUT,
	DELETE,
	PATCH,
	HEAD
};
typedef yaal::hcore::HExceptionT<HTTP> HHTTPException;

class HRequest {
public:
	class HHeader {
	private:
		yaal::hcore::HString _name;
		yaal::hcore::HString _value;
	public:
		HHeader( yaal::hcore::HString const& name_ = yaal::hcore::HString(), yaal::hcore::HString const& value_ = yaal::hcore::HString() )
			: _name( name_ )
			, _value( value_ ) {
		}
		HHeader& name( yaal::hcore::HString const& name_ ) {
			_name = name_;
			return ( *this );
		}
		yaal::hcore::HString const& name( void ) const {
			return ( _name );
		}
		HHeader& value( yaal::hcore::HString const& value_ ) {
			_value = value_;
			return ( *this );
		}
		yaal::hcore::HString const& value( void ) const {
			return ( _value );
		}
	};
	class HPayload {
	private:
		yaal::hcore::HStreamInterface* _stream;
		yaal::hcore::HString _mimeType;
		mutable yaal::hcore::HString _filename;
	public:
		HPayload(
			yaal::hcore::HStreamInterface* stream_ = nullptr,
			yaal::hcore::HString const& mimeType_ = yaal::hcore::HString(),
			yaal::hcore::HString const& filename_ = yaal::hcore::HString()
		) : _stream( stream_ )
			, _mimeType( mimeType_ )
			, _filename( filename_ ) {
		}
		HPayload( HPayload const& ) = default;
		HPayload& operator = ( HPayload const& ) = default;
		HPayload& stream( yaal::hcore::HStreamInterface& stream_ ) {
			_stream = &stream_;
			return ( *this );
		}
		yaal::hcore::HStreamInterface* stream( void ) const {
			return ( _stream );
		}
		HPayload& mime_type( yaal::hcore::HString const& mimeType_ ) {
			_mimeType = mimeType_;
			return ( *this );
		}
		yaal::hcore::HString const& mime_type( void ) const {
			return ( _mimeType );
		}
		HPayload& filename( yaal::hcore::HString const& filename_ ) {
			_filename = filename_;
			return ( *this );
		}
		yaal::hcore::HString const& filename( void ) const;
	};
	typedef yaal::hcore::HArray<HHeader> headers_t;
	typedef yaal::hcore::HArray<HPayload> payloads_t;
private:
	HTTP _type;
	yaal::hcore::HString _url;
	headers_t _headers;
	payloads_t _payloads;
	yaal::hcore::HString _login;
	yaal::hcore::HString _password;
	yaal::hcore::HString _userAgent;
public:
	HRequest(
		HTTP type_ = HTTP::GET,
		yaal::hcore::HString const& url_ = yaal::hcore::HString(),
		yaal::hcore::HString const& login_ = yaal::hcore::HString(),
		yaal::hcore::HString const& password_ = yaal::hcore::HString(),
		payloads_t const& payloads_ = payloads_t(),
		headers_t const& headers_ = headers_t(),
		yaal::hcore::HString const& userAgent_ = yaal::hcore::HString()
	) : _type( type_ )
		, _url( url_ )
		, _headers( headers_ )
		, _payloads( payloads_ )
		, _login( login_ )
		, _password( password_ )
		, _userAgent( userAgent_ ) {
		return;
	}
	HRequest& type( HTTP type_ ) {
		_type = type_;
		return ( *this );
	}
	HTTP type( void ) const {
		return ( _type );
	}
	HRequest& url( yaal::hcore::HString const& url_ ) {
		_url = url_;
		return ( *this );
	}
	yaal::hcore::HString const& url( void ) const {
		return ( _url );
	}
	HRequest& login( yaal::hcore::HString const& login_ ) {
		_login = login_;
		return ( *this );
	}
	yaal::hcore::HString const& login( void ) const {
		return ( _login );
	}
	HRequest& password( yaal::hcore::HString const& password_ ) {
		_password = password_;
		return ( *this );
	}
	yaal::hcore::HString const& password( void ) const {
		return ( _password );
	}
	HRequest& header( HHeader const& header_ ) {
		_headers.push_back( header_ );
		return ( *this );
	}
	headers_t const& headers( void ) const {
		return ( _headers );
	}
	HRequest& payload( HPayload const& payload_ ) {
		_payloads.push_back( payload_ );
		return ( *this );
	}
	payloads_t const& payloads( void ) const {
		return ( _payloads );
	}
	HRequest& user_agent( yaal::hcore::HString const& userAgent_ ) {
		_userAgent = userAgent_;
		return ( *this );
	}
	yaal::hcore::HString const& user_agent( void ) const {
		return ( _userAgent );
	}
private:
	HRequest( HRequest const& ) = delete;
	HRequest& operator = ( HRequest const& ) = delete;
};

class HResponse {
private:
	yaal::hcore::HStreamInterface::ptr_t _stream;
	yaal::hcore::HTime _lastModified;
	yaal::hcore::HString _mimeType;
	int _contentLength;
	yaal::hcore::HString _filename;
public:
	HResponse(
		yaal::hcore::HStreamInterface::ptr_t const& stream_,
		yaal::hcore::HTime const& lastModified_,
		yaal::hcore::HString const& mimeType_,
		int contentLength_,
		yaal::hcore::HString const& filename_
	) : _stream( stream_ )
		, _lastModified( lastModified_ )
		, _mimeType( mimeType_ )
		, _contentLength( contentLength_ )
		, _filename( filename_ ) {
	}
	yaal::hcore::HStreamInterface& stream( void ) {
		return ( *_stream );
	}
	yaal::hcore::HTime const& last_modified( void ) const {
		return ( _lastModified );
	}
	yaal::hcore::HString const& mime_type( void ) const {
		return ( _mimeType );
	}
	int content_length( void ) const {
		return ( _contentLength );
	}
	yaal::hcore::HString const& filename( void ) const {
		return ( _filename );
	}
	yaal::hcore::HStreamInterface::ptr_t sock( void ) const {
		return ( _stream );
	}
};

HResponse call( HRequest const& );

}

}

}

#endif /* #ifndef YAAL_TOOLS_HTTP_HXX_INCLUDED */

