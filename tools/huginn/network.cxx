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

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HNetwork : public HPackage {
	struct OPERATIONS {
		static int const READING = 1;
		static int const WRITING = 2;
	};
public:
	HNetwork( HClass* class_ )
		: HPackage( class_ ) {
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
private:
	HHuginn::value_t do_connect( huginn::HThread* thread_, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		char const name[] = "Network.connect";
		verify_arg_count( name, values_, 1, 2, thread_, position_ );
		ARITY arity( values_.get_size() > 1 ? ARITY::MULTIPLE : ARITY::UNARY );
		verify_arg_type( name, values_, 0, HHuginn::TYPE::STRING, arity, thread_, position_ );
		verify_arg_type( name, values_, 1, HHuginn::TYPE::INTEGER, arity, thread_, position_ );
		int port( 0 );
		if ( arity == ARITY::MULTIPLE ) {
			port = static_cast<int>( get_integer( values_[1] ) );
			if ( ( port <= 0 ) || ( port > 65535 ) ) {
				thread_->raise( exception_class(), "Bad port: "_ys.append( port ), position_ );
				port = -1;
			}
		}
		HHuginn::value_t v( thread_->runtime().none_value() );
		if ( port >= 0 ) {
			HStreamInterface::ptr_t stream( make_pointer<HSocket>( port > 0 ? HSocket::TYPE::NETWORK : HSocket::TYPE::FILE ) );
			HSocket* s( static_cast<HSocket*>( stream.raw() ) );
			try {
				s->connect( get_string( values_[0] ), port );
				HObjectFactory& of( thread_->object_factory() );
				v = of.create<HStream>( of.stream_class(), stream );
			} catch ( HResolverException const& e ) {
				thread_->raise( exception_class(), e.what(), position_ );
			} catch ( HSocketException const& e ) {
				thread_->raise( exception_class(), e.what(), position_ );
			}
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
		{ "connect", runtime_->create_method( &HNetwork::connect ), "( *host*, *port* ) - create a TCP connection to given *host* at given *port*" },
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

