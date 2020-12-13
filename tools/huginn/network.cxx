/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/huginn/stream.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "hcore/hsocket.hxx"
#include "hcore/resolver.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"
#include "enumeration.hxx"
#include "time.hxx"
#include "json.hxx"
#include "tools/hstringstream.hxx"
#include "tools/http.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

namespace {

inline int socket_type_to_idx( HSocket::socket_type_t socketType_ ) {
	int idx( -1 );
	if ( socketType_ & HSocket::TYPE::SSL ) {
		idx = 1;
	} else if ( socketType_ & HSocket::TYPE::FILE ) {
		idx = 2;
	} else {
		idx = 0;
	}
	return ( idx );
}

inline HSocket::socket_type_t connection_type_to_socket_type( HHuginn::value_t const& connectionType_ ) {
	u64_t connTypeEnumRawVal( static_cast<u64_t>( get_enumeral( connectionType_ ) ) );
	HSocket::socket_type_t socketType( HSocket::TYPE::DEFAULT | HSocket::TYPE::CLIENT | HSocket::TYPE::BLOCKING );
	if ( connTypeEnumRawVal == HSocket::TYPE::NETWORK.value() ) {
		socketType |= ( HSocket::TYPE::NETWORK );
	} else if ( connTypeEnumRawVal == HSocket::TYPE::SSL.value() ) {
		socketType |= ( HSocket::TYPE::NETWORK | HSocket::TYPE::SSL );
	} else {
		socketType |= HSocket::TYPE::FILE;
	}
	return ( socketType );
}

}

class HNetwork : public HPackage {
	enumeration::HEnumerationClass::ptr_t _connectionTypeClass;
	HHuginn::class_t _httpResponseClass;
public:
	HNetwork( HClass* class_ )
		: HPackage( class_ )
		, _connectionTypeClass(
			add_enumeration_as_member(
				class_,
				enumeration::create_class(
					class_->runtime(),
					"CONNECTION_TYPE",
					enumeration::descriptions_t{
						{ "PLAIN", "a plain (unencrypted) connection to network socket", safe_int::cast<int>( HSocket::TYPE::NETWORK.value() ) },
						{ "SSL",   "a SSL encrypted connection to network socket", safe_int::cast<int>( HSocket::TYPE::SSL.value() ) },
						{ "FILE",  "a connection to Unix Domain Socket", safe_int::cast<int>( HSocket::TYPE::FILE.value() ) },
					},
					"The `CONNECTION_TYPE` enumeration is a set of possible connection types.",
					HHuginn::VISIBILITY::PACKAGE,
					class_
				),
				"a set of possible connection types."
			)
		)
		, _httpResponseClass(
			add_class_as_member(
				class_,
				create_class(
					class_->runtime(),
					"HTTPResponse",
					HHuginn::field_definitions_t{
						{ "stream",       class_->runtime()->none_value(), "a HTTP connection stream to fetch requested resource" },
						{ "mimeType",     class_->runtime()->none_value(), "a mime type of acquired resource" },
						{ "lastModified", class_->runtime()->none_value(), "a time of last modification of acquired resource" },
						{ "filename",     class_->runtime()->none_value(), "a suggested filename for acquired resource" }
					},
					"The `HTTPResponse` class is representing a response to `Network.get(...)` request",
					HHuginn::VISIBILITY::PACKAGE,
					class_
				),
				"a response to `Network.get(...)` request"
			)
		) {
		return;
	}
	static HHuginn::value_t resolve( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Network.resolve", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		hcore::HString s;
		try {
			s = resolver::ip_to_string( resolver::get_ip( get_string( values_[0] ) ) );
		} catch ( HResolverException const& e ) {
			thread_->raise( static_cast<HNetwork*>( object_->raw() )->exception_class(), e.what(), position_ );
		}
		return ( thread_->object_factory().create_string( yaal::move( s ) ) );
		M_EPILOG
	}
	static HHuginn::value_t connect( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		return ( static_cast<HNetwork*>( object_->raw() )->do_connect( thread_, values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t get( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		return ( static_cast<HNetwork*>( object_->raw() )->do_get( thread_, values_, position_ ) );
		M_EPILOG
	}
	static HHuginn::value_t post( huginn::HThread* thread_, HHuginn::value_t* object_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		return ( static_cast<HNetwork*>( object_->raw() )->do_post( thread_, values_, position_ ) );
		M_EPILOG
	}
private:
	HHuginn::value_t do_connect( huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[][24] = {
			"Network.connect",
			"Network.connect[PLAIN]",
			"Network.connect[SSL]",
			"Network.connect[FILE]"
		};
		verify_arg_count( name[0], values_, 2, 3, thread_, position_ );
		verify_arg_type( name[0], values_, 0, _connectionTypeClass->enumeral_class(), ARITY::MULTIPLE, thread_, position_ );
		HSocket::socket_type_t socketType( connection_type_to_socket_type( values_[0] ) );
		int socketTypeIdx( socket_type_to_idx( socketType ) + 1 );
		int argCount( 2 + ( socketType & HSocket::TYPE::FILE ? 0 : 1 ) );
		verify_arg_count( name[socketTypeIdx], values_, argCount, argCount, thread_, position_ );
		verify_arg_type( name[socketTypeIdx], values_, 1, HHuginn::TYPE::STRING, ARITY::MULTIPLE, thread_, position_ );
		int port( 0 );
		if ( ! ( socketType & HSocket::TYPE::FILE ) ) {
			verify_arg_type( name[socketTypeIdx], values_, 2, HHuginn::TYPE::INTEGER, ARITY::MULTIPLE, thread_, position_ );
			port = static_cast<int>( get_integer( values_[2] ) );
			if ( ( port <= 0 ) || ( port > 65535 ) ) {
				thread_->raise( exception_class(), "Bad port: "_ys.append( port ), position_ );
				port = -1;
			}
		}
		HHuginn::value_t v( thread_->runtime().none_value() );
		if ( port >= 0 ) {
			HStreamInterface::ptr_t stream( make_pointer<HSocket>( socketType ) );
			HSocket* s( static_cast<HSocket*>( stream.raw() ) );
			try {
				s->connect( get_string( values_[1] ), port );
				HObjectFactory& of( thread_->object_factory() );
				v = of.create<HStream>( of.stream_class(), stream );
			} catch ( HResolverException const& e ) {
				thread_->raise( exception_class(), e.what(), position_ );
			} catch ( HSocketException const& e ) {
				thread_->raise( exception_class(), e.what(), position_ );
			} catch ( HOpenSSLException const& e ) {
				thread_->raise( exception_class(), e.what(), position_ );
			}
		}
		return ( v );
		M_EPILOG
	}
	HHuginn::value_t do_get( huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HParameter namedParameters[] = {
			{ "login", HHuginn::TYPE::STRING },
			{ "password", HHuginn::TYPE::STRING },
			{}
		};
		char const name[] = "Network.get";
		verify_named_parameters( name, values_, namedParameters, thread_, position_ );
		verify_signature( name, values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HHuginn::value_t v( thread_->runtime().none_value() );
		try {
			http::HRequest request( http::HTTP::GET, get_string( values_[0] ) );
			if ( !! namedParameters[0].value() ) {
				request.login( get_string( namedParameters[0].value() ) );
			}
			if ( !! namedParameters[1].value() ) {
				request.password( get_string( namedParameters[1].value() ) );
			}
			http::HResponse response( http::call( request ) );
			HObjectFactory& of( thread_->object_factory() );
			HObject::fields_t fields;
			fields.push_back( of.create<HStream>( of.stream_class(), response.sock() ) );
			fields.push_back( of.create_string( response.mime_type() ) );
			fields.push_back( of.create<HTime>( of.time_class(), response.last_modified() ) );
			fields.push_back( of.create_string( response.filename() ) );
			v = of.create_object( _httpResponseClass.raw(), fields );
		} catch ( HResolverException const& e ) {
			thread_->raise( exception_class(), e.what(), position_ );
		} catch ( HSocketException const& e ) {
			thread_->raise( exception_class(), e.what(), position_ );
		} catch ( HOpenSSLException const& e ) {
			thread_->raise( exception_class(), e.what(), position_ );
		} catch ( http::HHTTPException const& e ) {
			thread_->raise( exception_class(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
	HHuginn::value_t do_post( huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		HParameter namedParameters[] = {
			{ "login", HHuginn::TYPE::STRING },
			{ "password", HHuginn::TYPE::STRING },
			{}
		};
		char const name[] = "Network.post";
		verify_named_parameters( name, values_, namedParameters, thread_, position_ );
		verify_arg_count( name, values_, 2, meta::max_signed<int>::value, thread_, position_ );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, ARITY::MULTIPLE, thread_, position_ );
		HHuginn::value_t v( thread_->runtime().none_value() );
		try {
			HObjectFactory& of( thread_->object_factory() );
			http::HRequest request( http::HTTP::POST, get_string( values_[0] ) );
			int argCount( static_cast<int>( values_.get_size() ) );
			http::HRequest::payloads_t payloads;
			typedef yaal::hcore::HArray<yaal::hcore::HStreamInterface::ptr_t> json_streams_t;
			json_streams_t jsonStreams;
			for ( int i( 1 ); i < argCount; ++ i ) {
				if ( values_[i]->get_class() == of.stream_class() ) {
					request.payload( static_cast<HStream*>( values_[i].raw() )->raw().raw() );
				} else {
					jsonStreams.push_back( make_pointer<HStringStream>() );
					json_serialize( thread_, values_[i], *jsonStreams.back(), false, position_ );
					request.payload( http::HRequest::HPayload( jsonStreams.back().raw(), "application/json" ) );
				}
			}
			if ( !! namedParameters[0].value() ) {
				request.login( get_string( namedParameters[0].value() ) );
			}
			if ( !! namedParameters[1].value() ) {
				request.password( get_string( namedParameters[1].value() ) );
			}
			http::HResponse response( http::call( request ) );
			HObject::fields_t fields;
			fields.push_back( of.create<HStream>( of.stream_class(), response.sock() ) );
			fields.push_back( of.create_string( response.mime_type() ) );
			fields.push_back( of.create<HTime>( of.time_class(), response.last_modified() ) );
			fields.push_back( of.create_string( response.filename() ) );
			v = of.create_object( _httpResponseClass.raw(), fields );
		} catch ( HResolverException const& e ) {
			thread_->raise( exception_class(), e.what(), position_ );
		} catch ( HSocketException const& e ) {
			thread_->raise( exception_class(), e.what(), position_ );
		} catch ( HOpenSSLException const& e ) {
			thread_->raise( exception_class(), e.what(), position_ );
		} catch ( http::HHTTPException const& e ) {
			thread_->raise( exception_class(), e.what(), position_ );
		}
		return ( v );
		M_EPILOG
	}
};

namespace package_factory {

class HNetworkCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} networkCreator;

HPackageCreatorInterface::HInstance HNetworkCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Network",
			"The `Network` package provides access to various networking related functionalities.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "connect", runtime_->create_method( &HNetwork::connect ), "( *connectionType*, *target*[, *port*] ) - create a TCP connection of type *connectionType* to given *target*, optionally at given *port*" },
		{ "get",     runtime_->create_method( &HNetwork::get ),     "( *url*[, *login*:*{login}*, *password*:*{pass}*] ) - fetch a resource from a remote HTTP server" },
		{ "post",    runtime_->create_method( &HNetwork::post ),    "( *url*, *payload1*, [*payload2*, ..., *login*:*{login}*, *password*:*{pass}*] ) - push *payloads* to a remote HTTP server" },
		{ "resolve", runtime_->create_method( &HNetwork::resolve ), "( *hostName* ) - resolve IP address of given *hostName*" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HNetwork>( c.raw() ) };
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "Network", &networkCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

