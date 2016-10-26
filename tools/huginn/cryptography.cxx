/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  cryptography.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

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
using namespace yaal::tools::huginn;

namespace yaal {

namespace tools {

namespace huginn {

class HCryptography : public HHuginn::HObject {
	HHuginn::class_t _exceptionClass;
public:
	HCryptography( HHuginn::HClass* class_ )
		: HObject( class_ )
		, _exceptionClass( exception::create_class( class_->runtime(), "CryptographyException", "The `CryptographyException` exception type for `Cryptography` package." ) ) {
		return;
	}
	static HHuginn::value_t hash( char const* name_, tools::hash::hash_string_t hash_, huginn::HThread* thread_, HHuginn::value_t*, HHuginn::values_t const& values_, int position_ ) {
		M_PROLOG
		verify_arg_count( name_, values_, 1, 1, position_ );
		verify_arg_type( name_, values_, 0, HHuginn::TYPE::STRING, true, position_ );
		return ( thread_->object_factory().create_string( hash_( get_string( values_[0] ) ) ) );
		M_EPILOG
	}
};

namespace package_factory {

class HCryptographyCreator : public HPackageCreatorInterface {
protected:
	virtual HHuginn::value_t do_new_instance( HRuntime* );
} cryptographyCreator;

HHuginn::value_t HCryptographyCreator::do_new_instance( HRuntime* runtime_ ) {
	M_PROLOG
	HHuginn::class_t c(
		runtime_->create_class(
			"Cryptography",
			nullptr,
			HHuginn::field_definitions_t{
				{ "md5", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HCryptography::hash, "Cryptography.md5", static_cast<tools::hash::hash_string_t>( &tools::hash::md5 ), _1, _2, _3, _4 ) ), "( *str* ) - calculare *MD5* sum of given `string`" },
				{ "sha1", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HCryptography::hash, "Cryptography.sha1", static_cast<tools::hash::hash_string_t>( &tools::hash::sha1 ), _1, _2, _3, _4 ) ), "( *str* ) - calculare *SHA1* sum of given `string`" },
				{ "sha512", make_pointer<HHuginn::HClass::HMethod>( hcore::call( &HCryptography::hash, "Cryptography.sha512", static_cast<tools::hash::hash_string_t>( &tools::hash::sha512 ), _1, _2, _3, _4 ) ), "( *str* ) - calculare *SHA512* sum of given `string`" }
			},
			"The `Cryptography` package provides functionality of a cryptographical nature, like hashing functions."
		)
	);
	runtime_->huginn()->register_class( c );
	return ( make_pointer<HCryptography>( c.raw() ) );
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

