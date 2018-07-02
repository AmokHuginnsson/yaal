/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_BREAK_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_BREAK_HXX_INCLUDED 1

#include "tools/huginn/statement.hxx"
#include "tools/huginn/frame.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HThread;

class HBreak : public HStatement {
public:
	typedef HBreak this_type;
	typedef HStatement base_type;
private:
	HFrame::STATE _state;
public:
	HBreak( HFrame::STATE, int, executing_parser::range_t );
protected:
	virtual void do_execute( HThread* ) const override;
private:
	HBreak( HBreak const& ) = delete;
	HBreak& operator = ( HBreak const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_BREAK_HXX_INCLUDED */

