/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_WHILE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_WHILE_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/scope.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HWhile : public HVirtualScope {
public:
	typedef HWhile this_type;
	typedef HStatement base_type;
private:
	HHuginn::expression_t _condition;
	HHuginn::scope_t _loop;
public:
	HWhile( HHuginn::statement_id_t, int, executing_parser::range_t );
	void init( HHuginn::expression_t const&, HHuginn::scope_t const& );
protected:
	virtual void do_execute_internal( HThread* ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_WHILE_HXX_INCLUDED */

