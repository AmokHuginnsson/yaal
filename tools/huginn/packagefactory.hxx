/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/huginn/packagefactory.hxx
 * \brief Declaration of HPackageCreatorInterface and HPackageFactory classes.
 */

#ifndef YAAL_TOOLS_HUGINN_PACKAGEFACTORY_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_PACKAGEFACTORY_HXX_INCLUDED

#include "hcore/hmap.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/hsingleton.hxx"

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

class HPlugin;

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
	typedef yaal::hcore::HPointer<HPlugin> plugin_t;
	typedef yaal::hcore::HArray<plugin_t> plugins_t;
	typedef yaal::hcore::HSet<yaal::hcore::HString> visited_imports_t;
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
	plugins_t _binaries;
	visited_imports_t _visitedImports;
public:
	void register_package_creator( yaal::hcore::HString const&, HPackageCreatorInterface* );
	HHuginn::value_t create_package( HRuntime*, yaal::hcore::HString const&, int );
	creators_t::iterator begin( void );
	creators_t::iterator end( void );
	void initialize_globals( void );
	void cleanup_globals( void );
private:
	HHuginn::value_t load_binary( HRuntime*, HHuginn::paths_t const&, yaal::hcore::HString const&, int );
	HHuginn::value_t load_module( HRuntime*, HHuginn::paths_t const&, yaal::hcore::HString const&, int );
	HHuginn::value_t compile_module( HRuntime*, HHuginn::paths_t const&, yaal::hcore::HString const&, yaal::hcore::HString const&, int );
	HPackageFactory( void );
	~HPackageFactory( void );
	static int life_time( int );
	friend class yaal::hcore::HSingleton<HPackageFactory>;
	friend class yaal::hcore::HDestructor<HPackageFactory>;
};

}

}

}

#ifdef __HOST_OS_TYPE_CYGWIN__
/*
 * Required for proper auto-registration of packages from libraries other than yaal_tools.
 */
M_YAAL_TOOLS_EXTERN template class yaal::hcore::HSingleton<yaal::tools::huginn::HPackageFactory>;
#endif /* #ifdef __HOST_OS_TYPE_CYGWIN__ */

#endif /* not YAAL_TOOLS_HUGINN_PACKAGEFACTORY_HXX_INCLUDED */

