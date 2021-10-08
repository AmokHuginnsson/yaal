/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/hyaml.hxx
 * \brief Declaration of HYAML class.
 */

#ifndef YAAL_TOOLS_HYAML_HXX_INCLUDED
#define YAAL_TOOLS_HYAML_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hstreaminterface.hxx"
#include "tools/model.hxx"

namespace yaal {

namespace tools {

class HYAML {
public:
	typedef HYAML this_type;
	typedef yaal::hcore::HArray<yaal::tools::model::HValue> documents_t;
private:
	documents_t _documents;
public:
	HYAML( void );
	HYAML( HYAML const& ) = default;
	HYAML( HYAML&& ) = default;
	HYAML& operator = ( HYAML const& ) = default;
	HYAML& operator = ( HYAML&& ) = default;
	bool operator == ( HYAML const& ) const;
	bool operator != ( HYAML const& other_ ) const {
		return ( ! operator == ( other_ ) );
	}
	documents_t& documents( void );
	yaal::tools::model::HValue& add_document( void );
	void load( yaal::hcore::HStreamInterface& );
	void save( yaal::hcore::HStreamInterface& ) const;
	void clear( void );
};

typedef yaal::hcore::HExceptionT<HYAML> HYAMLException;

}

}

#endif /* #ifndef YAAL_TOOLS_HYAML_HXX_INCLUDED */

