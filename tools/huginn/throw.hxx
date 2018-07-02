/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_THROW_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_THROW_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/statement.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HThrow : public HStatement {
public:
	typedef HThrow this_type;
	typedef HStatement base_type;
private:
	HHuginn::expression_t _expression;
public:
	HThrow( HHuginn::expression_t const&, int, executing_parser::range_t );
protected:
	virtual void do_execute( HThread* ) const override;
private:
	HThrow( HThrow const& ) = delete;
	HThrow& operator = ( HThrow const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_THROW_HXX_INCLUDED */

