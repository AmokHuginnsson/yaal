/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/observer.hxx
 * \brief Declaration of huginn::HObserver class.
 */

#ifndef YAAL_TOOLS_HUGINN_OBSERVER_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_OBSERVER_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HObserver : public HValue {
public:
	typedef HObserver this_type;
	typedef HValue base_type;
	typedef HHuginn::value_ref_t value_type;
private:
	value_type _value;
public:
	HObserver( HClass const*, HHuginn::value_t const& );
	HHuginn::value_t value( void ) const;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_OBSERVER_HXX_INCLUDED */

