/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "thread.hxx"
#include "stream.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "tools/base64.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"
#include "tools/hstringstream.hxx"
#include "tools/hmemory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HBase64 : public HPackage {
public:
	HBase64( HClass* class_ )
		: HPackage( class_ ) {
		return;
	}
	static HHuginn::value_t encode( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Base64.encode", values_, { HHuginn::TYPE::BLOB }, thread_, position_ );
		HChunk const& blob( static_cast<HBlob*>( values_[0].raw() )->value() );
		HMemory ms( make_resource<HMemoryObserver>( const_cast<void*>( blob.raw() ), blob.size() ) );
		HStringStream ss;
		base64::encode( ms, ss );
		return ( thread_->object_factory().create_string( yaal::move( ss.str() ) ) );
		M_EPILOG
	}
	static HHuginn::value_t decode( huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( "Base64.decode", values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		HChunk workBuffer;
		HMemoryProvider* mp( nullptr );
		HMemory::memory_strategy_interface_t mpRef( make_resource<HMemoryProvider>( workBuffer, 0 ) );
		mp = static_cast<HMemoryProvider*>( mpRef.get() );
		HMemory ms( yaal::move( mpRef ) );
		HStringStream ss( get_string( values_[0] ) );
		base64::decode( ss, ms );
		HChunk data;
		if ( mp->get_size() > 0 ) {
			data.realloc( mp->get_size(), HChunk::STRATEGY::EXACT );
			::memcpy( data.raw(), workBuffer.raw(), static_cast<size_t>( mp->get_size() ) );
		}
		return ( thread_->object_factory().create_blob( yaal::move( data ) ) );
		M_EPILOG
	}
private:
};

namespace package_factory {

class HBase64Creator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} base64Creator;

HPackageCreatorInterface::HInstance HBase64Creator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Base64",
			"The `Base64` package provides Base64 based encoding/decoding functionality.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "encode", runtime_->create_method( &HBase64::encode ), "( *blob* ) - base64 encode *blob* of data" },
		{ "decode", runtime_->create_method( &HBase64::decode ), "( *base64str* ) - base64 decode *base64str*" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HBase64>( c.raw() ) };
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "Base64", &base64Creator );
	return failed;
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

