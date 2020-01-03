/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_COMPILEDREGULAREXPRESSION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_COMPILEDREGULAREXPRESSION_HXX_INCLUDED 1

#include "hcore/hresource.hxx"
#include "hcore/hregex.hxx"
#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HCompiledRegularExpression : public HValue {
public:
	typedef yaal::hcore::HResource<yaal::hcore::HRegex> regex_t;
private:
	regex_t _regex;
public:
	HCompiledRegularExpression( HClass const*, regex_t );
	static HHuginn::value_t match( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t groups( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t replace( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime*, HClass*, HClass const* );
private:
	HHuginn::value_t do_match( huginn::HThread*, HHuginn::values_t&, int );
	HHuginn::value_t do_groups( huginn::HThread*, HHuginn::values_t&, int );
	HHuginn::value_t do_replace( huginn::HThread*, HHuginn::values_t&, int );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_COMPILEDREGULAREXPRESSION_HXX_INCLUDED */

