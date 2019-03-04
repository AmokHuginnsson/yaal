/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_BOOLEANEVALUATOR_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_BOOLEANEVALUATOR_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"
#include "tools/huginn/operator.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HBooleanEvaluator : public huginn::HValue {
public:
	typedef HBooleanEvaluator this_type;
	typedef huginn::HValue base_type;
	typedef yaal::hcore::HArray<HHuginn::expression_t> expressions_t;
private:
	expressions_t _expressions;
	OPERATOR _operator;
public:
	HBooleanEvaluator( HRuntime*, expressions_t const&, OPERATOR );
	bool execute( huginn::HThread* );
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_BOOLEANEVALUATOR_HXX_INCLUDED */

