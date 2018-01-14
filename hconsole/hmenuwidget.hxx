/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_HMENUWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HMENUWIDGET_HXX_INCLUDED 1

#include "hconsole/htreewidget.hxx"
#include "hconsole/omenuitem.hxx"
#include "hcore/hpointer.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Implementation of TUI Menu control class.
 *
 * Menu control displays menu and automatically spawns appropriate actions.
 */
class HMenuWidget : public HTreeWidget {
public:
	typedef HMenuWidget this_type;
	typedef HTreeWidget base_type;
	typedef yaal::hcore::HPointer<HMenuWidget> ptr_t;
	typedef HAsIsValueTreeModel<OMenuItem> menu_model_t;
	typedef menu_model_t::data_t data_t;
	typedef menu_model_t::data_ptr_t data_ptr_t;
protected:
	HTUIProcess* _process;
public:
	HMenuWidget( HWindow* parent,
			int row,
			int col,
			int height,
			int width,
			yaal::hcore::HString const& label );
	virtual ~HMenuWidget( void );
	void init( HTUIProcess* );
	void set_data( data_ptr_t );
protected:
	OMenuItem const* get_selected( void );
	virtual int do_process_input( int ) override;
	virtual bool do_click( mouse::OMouse& ) override;
private:
	HMenuWidget( HMenuWidget const& );
	HMenuWidget& operator = ( HMenuWidget const& );
};

typedef yaal::hcore::HExceptionT<HMenuWidget, HTreeWidgetException> HMenuWidgetException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HMENUWIDGET_HXX_INCLUDED */

