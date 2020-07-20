/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_FOR_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_FOR_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/scope.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HFor : public huginn::HVirtualScope {
public:
	typedef HFor this_type;
	typedef HStatement base_type;
private:
	HHuginn::expressions_t _control;
	HHuginn::expression_t _source;
	HHuginn::scope_t _loop;
	bool _needsFrame;
public:
	HFor( HHuginn::statement_id_t, HHuginn::expressions_t&&, HHuginn::expression_t const&, HHuginn::scope_t const&, bool, int, executing_parser::range_t );
protected:
	virtual void do_execute_internal( HThread* ) const override;
private:
	void run_loop( HThread*, HFrame*, HHuginn::value_t&& ) const;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FOR_HXX_INCLUDED */

