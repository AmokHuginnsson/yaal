/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_IF_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_IF_HXX_INCLUDED 1

#include "compiler.hxx"
#include "tools/huginn/scope.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HIf : public HVirtualScope {
public:
	typedef HIf this_type;
	typedef HStatement base_type;
	typedef OCompiler::OScopeContext::active_scopes_t if_clauses_t;
private:
	if_clauses_t _ifClauses;
	HHuginn::scope_t _elseClause;
public:
	HIf( HHuginn::statement_id_t, int, executing_parser::range_t );
	void init( if_clauses_t const&, HHuginn::scope_t const& );
protected:
	virtual void do_execute_internal( HThread* ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_IF_HXX_INCLUDED */

