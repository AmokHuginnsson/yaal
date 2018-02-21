/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_INTROEXPRESSION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_INTROEXPRESSION_HXX_INCLUDED 1

#include "tools/huginn/expression.hxx"
#include "runtime.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HIntroExpression : public HExpression {
public:
	typedef HIntroExpression this_type;
	typedef HExpression base_type;
private:
	HIntrospectorInterface* _introspector;
public:
	HIntroExpression( HIntrospectorInterface*, int = MAIN_FILE_ID, int = 0 );
	void get_variable_reference( OExecutionStep const&, huginn::HFrame* );
	void make_variable( OExecutionStep const&, huginn::HFrame* );
protected:
	virtual void do_execute( huginn::HThread* ) const override;
private:
	HIntroExpression( HIntroExpression const& ) = delete;
	HIntroExpression& operator = ( HIntroExpression const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_INTROEXPRESSION_HXX_INCLUDED */

