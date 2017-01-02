/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  packagefactory.hxx - this file is integral part of `yaal' project.

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

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_PACKAGEFACTORY_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_PACKAGEFACTORY_HXX_INCLUDED

#include "hcore/hmap.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/hsingleton.hxx"

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HPackageCreatorInterface {
protected:
	virtual void do_initialize_globals( void ){};
	virtual void do_cleanup_globals( void ){};
	virtual HHuginn::value_t do_new_instance( HRuntime* ) = 0;
public:
	virtual ~HPackageCreatorInterface( void ){}
	void initialize_globals( void );
	void cleanup_globals( void );
	HHuginn::value_t new_instance( HRuntime* );
};

class M_YAAL_TOOLS_PUBLIC_API HPackageFactory : public yaal::hcore::HSingleton<HPackageFactory> {
public:
	typedef HPackageFactory this_type;
	typedef HSingleton<HPackageFactory> base_type;
	struct OCreator {
		OCreator( void )
			: _instantiator( nullptr ) {
		}
		OCreator( OCreator const& ) = default;
		OCreator& operator = ( OCreator const& ) = default;
		HPackageCreatorInterface* _instantiator;
	};
	typedef yaal::hcore::HMap<yaal::hcore::HString, OCreator> creators_t;
private:
	creators_t _creators;
public:
	void register_package_creator( yaal::hcore::HString const&, HPackageCreatorInterface* );
	HHuginn::value_t create_package( HRuntime*, HHuginn::paths_t const&, HHuginn::compiler_setup_t, yaal::hcore::HString const&, int );
	creators_t::iterator begin( void );
	creators_t::iterator end( void );
	void initialize_globals( void );
	void cleanup_globals( void );
private:
	HHuginn::value_t load_module( HRuntime*, HHuginn::paths_t const&, HHuginn::compiler_setup_t, yaal::hcore::HString const&, yaal::hcore::HString const&, int );
	HPackageFactory( void );
	~HPackageFactory( void );
	static int life_time( int );
	friend class yaal::hcore::HSingleton<HPackageFactory>;
	friend class yaal::hcore::HDestructor<HPackageFactory>;
};

}

}

}

M_YAAL_TOOLS_EXTERN template class yaal::hcore::HSingleton<yaal::tools::huginn::HPackageFactory>;

#endif /* not YAAL_TOOLS_HUGINN_PACKAGEFACTORY_HXX_INCLUDED */

