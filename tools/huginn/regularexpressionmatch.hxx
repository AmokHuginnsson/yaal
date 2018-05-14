/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_REGULAREXPRESSIONMATCH_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_REGULAREXPRESSIONMATCH_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "compiledregularexpression.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HRegularExpressionMatch : public HHuginn::HIterable {
	HCompiledRegularExpression::regex_t _regex;
	HHuginn::value_t _string;
	yaal::hcore::HString const& _fast;
public:
	HRegularExpressionMatch( HHuginn::HClass const*, HCompiledRegularExpression::regex_t, HHuginn::value_t const& );
	yaal::hcore::HString const& get_string( void ) const;
	yaal::hcore::HRegex::HMatchIterator end( void ) const;
	static HHuginn::value_t matched( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime* );
protected:
	virtual iterator_t do_iterator( HThread*, int ) override;
	virtual int long do_size( huginn::HThread*, int ) const override;
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_REGULAREXPRESSIONMATCH_HXX_INCLUDED */

