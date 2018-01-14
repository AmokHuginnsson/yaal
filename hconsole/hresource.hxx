/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hconsole/hresource.hxx
 * \brief Declaration of HResource class.
 */

#ifndef YAAL_HCONSOLE_HRESOURCE_HXX_INCLUDED
#define YAAL_HCONSOLE_HRESOURCE_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/hpointer.hxx"
#include "tools/hxml.hxx"
#include "hconsole/hmenuwidget.hxx"

namespace yaal {

namespace hconsole {

class HTUIProcess;

class HResource {
public:
	typedef HResource this_type;
	typedef yaal::hcore::HPointer<HResource> ptr_t;
private:
	HTUIProcess* _tui;
	yaal::tools::HXml _xml;
public:
	HResource( HTUIProcess*, yaal::hcore::HString const& );
	void load( void );
	yaal::tools::xml::value_t get_value( yaal::hcore::HString const& ) const;
	yaal::tools::xml::value_t get_attribute( yaal::hcore::HString const& ) const;
private:
	void build_sub_menu( yaal::tools::HXml::HConstNodeProxy const&, HMenuWidget::data_t::node_t );
	void build_menu_item( yaal::tools::HXml::HConstNodeProxy const&, HMenuWidget::data_t::node_t );
	void create_window( yaal::hcore::HString const& );
	HResource( HResource const& );
	HResource& operator = ( HResource const& );
};

typedef yaal::hcore::HExceptionT<HResource> HResourceException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HRESOURCE_HXX_INCLUDED */

