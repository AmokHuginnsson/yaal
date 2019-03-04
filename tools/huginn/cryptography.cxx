/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "tools/hhuginn.hxx"
#include "runtime.hxx"
#include "tools/huginn/thread.hxx"
#include "tools/hash.hxx"
#include "helper.hxx"
#include "exception.hxx"
#include "packagefactory.hxx"
#include "objectfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools::hash;
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HCryptography : public huginn::HValue {
	HHuginn::class_t _exceptionClass;
public:
	HCryptography( huginn::HClass* class_ )
		: HValue( class_ )
		, _exceptionClass( class_exception( class_ ) ) {
		return;
	}
	static HHuginn::value_t hash( char const* name_, hash::FUNCTION hashType_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( name_, values_, { HHuginn::TYPE::STRING }, thread_, position_ );
		return ( thread_->object_factory().create_string( hash::string( hashType_, get_string( values_[0] ) ) ) );
		M_EPILOG
	}
	static HHuginn::value_t hmac( char const* name_, hash::FUNCTION hashType_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t& values_, int position_ ) {
		M_PROLOG
		verify_signature( name_, values_, { HHuginn::TYPE::STRING, HHuginn::TYPE::STRING }, thread_, position_ );
		return ( thread_->object_factory().create_string( hash::hmac( hashType_, get_string( values_[0] ), get_string( values_[1] ) ) ) );
		M_EPILOG
	}
};

namespace package_factory {

class HCryptographyCreator : public HPackageCreatorInterface {
protected:
	virtual HInstance do_new_instance( HRuntime* );
} cryptographyCreator;

HPackageCreatorInterface::HInstance HCryptographyCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Cryptography",
			"The `Cryptography` package provides functionality of a cryptographic nature, like hashing functions.",
			HHuginn::ACCESS::PRIVATE
		)
	);
	HHuginn::field_definitions_t fd{
		{ "md5",         runtime_->create_method( &HCryptography::hash, "Cryptography.md5",         hash::FUNCTION::MD5 ),    "( *str* ) - calculate *MD5* sum of given `string`" },
		{ "sha1",        runtime_->create_method( &HCryptography::hash, "Cryptography.sha1",        hash::FUNCTION::SHA1 ),   "( *str* ) - calculate *SHA1* sum of given `string`" },
		{ "sha512",      runtime_->create_method( &HCryptography::hash, "Cryptography.sha512",      hash::FUNCTION::SHA512 ), "( *str* ) - calculate *SHA512* sum of given `string`" },
		{ "hmac_md5",    runtime_->create_method( &HCryptography::hmac, "Cryptography.hmac_md5",    hash::FUNCTION::MD5 ),    "( *key*, *str* ) - calculate *HMAC-MD5* verification code for given `string` *str* using given *key*" },
		{ "hmac_sha1",   runtime_->create_method( &HCryptography::hmac, "Cryptography.hmac_sha1",   hash::FUNCTION::SHA1 ),   "( *key*, *str* ) - calculate *HMAC-SHA1* verification code for given `string` *str* using given *key*" },
		{ "hmac_sha512", runtime_->create_method( &HCryptography::hmac, "Cryptography.hmac_sha512", hash::FUNCTION::SHA512 ), "( *key*, *str* ) - calculate *HMAC-SHA512* verification code for given `string` *str* using given *key*" }
	};
	c->redefine( nullptr, fd );
	return { c, runtime_->object_factory()->create<HCryptography>( c.raw() ) };
	M_EPILOG
}

namespace {

bool registrar( void ) {
	M_PROLOG
	bool volatile failed = false;
	HPackageFactory& factory = HPackageFactory::get_instance();
	factory.register_package_creator( "Cryptography", &cryptographyCreator );
	return ( failed );
	M_EPILOG
}

bool volatile registered = registrar();

}

}

}

}

}

