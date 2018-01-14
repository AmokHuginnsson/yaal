/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_IF_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_IF_HXX_INCLUDED 1

#include "compiler.hxx"
#include "tools/huginn/statement.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HIf : public HStatement {
public:
	typedef HIf this_type;
	typedef HStatement base_type;
	typedef OCompiler::OScopeContext::active_scopes_t if_clauses_t;
private:
	if_clauses_t _ifClauses;
	HHuginn::scope_t _elseClause;
public:
	HIf( HStatement::statement_id_t, if_clauses_t const&, HHuginn::scope_t const&, int, int );
protected:
	virtual void do_execute( HThread* ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_IF_HXX_INCLUDED */

