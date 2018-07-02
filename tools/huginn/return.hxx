/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_RETURN_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_RETURN_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/statement.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HReturn : public HStatement {
public:
	typedef HReturn this_type;
	typedef HStatement base_type;
private:
	HHuginn::expression_t _expression;
public:
	HReturn( HHuginn::expression_t const&, int, executing_parser::range_t );
protected:
	virtual void do_execute( HThread* ) const override;
private:
	HReturn( HReturn const& ) = delete;
	HReturn& operator = ( HReturn const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_RETURN_HXX_INCLUDED */

