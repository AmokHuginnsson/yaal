/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_TERNARYEVALUATOR_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_TERNARYEVALUATOR_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HTernaryEvaluator : public HValue {
public:
	typedef HTernaryEvaluator this_type;
	typedef HValue base_type;
private:
	HHuginn::expression_t _condition;
	HHuginn::expression_t _ifTrue;
	HHuginn::expression_t _ifFalse;
public:
	HTernaryEvaluator( HHuginn::expression_t const&, HHuginn::expression_t const&, HHuginn::expression_t const& );
	value_t execute( huginn::HThread* );
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override M_DEBUG_CODE( __attribute__((__noreturn__)) );
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_TERNARYEVALUATOR_HXX_INCLUDED */

