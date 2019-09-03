/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_WHILE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_WHILE_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/statement.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HWhile : public HStatement {
public:
	typedef HWhile this_type;
	typedef HStatement base_type;
private:
	HHuginn::expression_t _condition;
	HHuginn::scope_t _loop;
	bool _needsFrame;
public:
	HWhile( HStatement::statement_id_t, HHuginn::expression_t const&, HHuginn::scope_t const&, bool, int, executing_parser::range_t );
protected:
	virtual void do_execute( HThread* ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_WHILE_HXX_INCLUDED */

