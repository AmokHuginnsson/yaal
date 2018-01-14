/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HPLUGIN_HXX_INCLUDED
#define YAAL_TOOLS_HPLUGIN_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/hpointer.hxx"

namespace yaal {

namespace tools {

/*! \brief Dynamically load and use shared libraries.
 */
class HPlugin final {
	typedef HPlugin this_type;
	void* _handle;
public:
	typedef yaal::hcore::HPointer<HPlugin> ptr_t;
	HPlugin( void );
	~HPlugin( void );
	void load( yaal::hcore::HString const& );
	void unload( void );
	bool is_loaded( void ) const;
	char const* error_message( int );
	template<typename name_t>
	void resolve( yaal::hcore::HString const&, name_t& );
	void* resolve( yaal::hcore::HString const& );
	template<typename name_t>
	void try_resolve( yaal::hcore::HString const&, name_t& );
	void* try_resolve( yaal::hcore::HString const& );
private:
	HPlugin( HPlugin const& );
	HPlugin const& operator = ( HPlugin const& );
};

template<typename name_t>
void HPlugin::resolve( yaal::hcore::HString const& name_, name_t& handle ) {
	M_PROLOG
	handle = bit_cast<name_t>( resolve( name_ ) );
	return;
	M_EPILOG
}

template<typename name_t>
void HPlugin::try_resolve( yaal::hcore::HString const& name_, name_t& handle ) {
	M_PROLOG
	handle = bit_cast<name_t>( try_resolve( name_ ) );
	return;
	M_EPILOG
}

typedef yaal::hcore::HExceptionT<HPlugin> HPluginException;

}

}

#endif /* #ifndef YAAL_TOOLS_HPLUGIN_HXX_INCLUDED */

