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

struct HTTP {};
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
	typedef yaal::hcore::HArray<HHeader> headers_t;
private:
	yaal::hcore::HString _url;
	yaal::hcore::HString _login;
	yaal::hcore::HString _password;
	headers_t _headers;
	yaal::hcore::HString _userAgent;
public:
	HRequest(
		yaal::hcore::HString const& url_ = yaal::hcore::HString(),
		yaal::hcore::HString const& login_ = yaal::hcore::HString(),
		yaal::hcore::HString const& password_ = yaal::hcore::HString(),
		headers_t const& headers_ = headers_t(),
		yaal::hcore::HString const& userAgent_ = yaal::hcore::HString()
	) : _url( url_ )
		, _login( login_ )
		, _password( password_ )
		, _headers( headers_ )
		, _userAgent( userAgent_ ) {
		return;
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
	HRequest& user_agent( yaal::hcore::HString const& userAgent_ ) {
		_userAgent = userAgent_;
		return ( *this );
	}
	yaal::hcore::HString const& user_agent( void ) const {
		return ( _userAgent );
	}
};

class HResponse {
private:
	yaal::hcore::HStreamInterface::ptr_t _stream;
	yaal::hcore::HTime _lastModified;
	yaal::hcore::HString _mimeType;
public:
	HResponse(
		yaal::hcore::HStreamInterface::ptr_t const& stream_,
		yaal::hcore::HTime const& lastModified_,
		yaal::hcore::HString const& mimeType_
	) : _stream( stream_ )
		, _lastModified( lastModified_ )
		, _mimeType( mimeType_ ) {
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
};

HResponse get( HRequest const& );

}

}

}

#endif /* #ifndef YAAL_TOOLS_HTTP_HXX_INCLUDED */

