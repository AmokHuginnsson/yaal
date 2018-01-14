/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hdata/hdatastatusbarwidget.hxx
 * \brief Declaration of HDataStatusBarWidget class.
 */

#ifndef YAAL_HDATA_HDATASTATUSBARWIDGET_HXX_INCLUDED
#define YAAL_HDATA_HDATASTATUSBARWIDGET_HXX_INCLUDED 1

#include "hconsole/hstatusbarwidget.hxx"
#include "hdata/hdatawindow.hxx"

namespace yaal {

namespace hdata {

/*! \brief Implementation of database enabled TUI Statusbar control class.
 *
 * DataStatusBar Control provides display of progress bars, allows
 * invocation of commands and specifying search patterns.
 * Additionally it provides document status presentation.
 */
class HDataStatusBarWidget : public hconsole::HStatusBarWidget {
public:
	HDataStatusBarWidget( HDataWindow*, yaal::hcore::HString const&, OAttribute const& = hconsole::_attributeStatusBar_ );
	virtual void do_draw_label ( void ) override;
};

}

}

#endif /* #ifndef YAAL_HDATA_HDATASTATUSBARWIDGET_HXX_INCLUDED */

