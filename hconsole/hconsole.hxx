/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_HCONSOLE_HCONSOLE_HXX_INCLUDED
#define YAAL_HCONSOLE_HCONSOLE_HXX_INCLUDED 1

#include "hconsole/hwidget.hxx"

namespace yaal {

namespace hconsole {

extern M_YAAL_HCONSOLE_PUBLIC_API int _latency_;
extern M_YAAL_HCONSOLE_PUBLIC_API tools::COLOR::color_t _screenBackground_;
extern M_YAAL_HCONSOLE_PUBLIC_API HWidget::OAttribute _attributeDisabled_;
extern M_YAAL_HCONSOLE_PUBLIC_API HWidget::OAttribute _attributeEnabled_;
extern M_YAAL_HCONSOLE_PUBLIC_API HWidget::OAttribute _attributeFocused_;
extern M_YAAL_HCONSOLE_PUBLIC_API HWidget::OAttribute _attributeStatusBar_;
extern M_YAAL_HCONSOLE_PUBLIC_API HWidget::OAttribute _attributeSearchHighlight_;
enum class USE_MOUSE {
	YES,
	NO,
	AUTO
};
extern M_YAAL_HCONSOLE_PUBLIC_API USE_MOUSE _useMouse_;
extern M_YAAL_HCONSOLE_PUBLIC_API char _commandComposeCharacter_;
extern M_YAAL_HCONSOLE_PUBLIC_API int _commandComposeDelay_;
void banner( void );

}

}

#endif /* #ifndef YAAL_HCONSOLE_HCONSOLE_HXX_INCLUDED */

