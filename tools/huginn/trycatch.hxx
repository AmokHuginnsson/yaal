/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_TRYCATCH_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_TRYCATCH_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/scope.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HTryCatch : public HVirtualScope {
public:
	typedef HTryCatch this_type;
	typedef HStatement base_type;
	class HCatch : public HVirtualScope {
		HHuginn::identifier_id_t _type;
		HHuginn::expression_t _control;
		HHuginn::scope_t _scope;
	public:
		HCatch( HHuginn::statement_id_t, HHuginn::identifier_id_t, int, executing_parser::range_t );
		void init(
			HHuginn::expression_t const&,
			HHuginn::scope_t const&
		);
		HHuginn::identifier_id_t type( void ) const {
			return ( _type );
		}
	private:
		virtual void do_execute_internal( HThread* ) const override;
	};
	typedef yaal::hcore::HArray<HHuginn::scope_t> catches_t;
private:
	HHuginn::scope_t _try;
	catches_t _catches;
public:
	HTryCatch( HHuginn::scope_t const&, catches_t&&, int, executing_parser::range_t );
protected:
	virtual void do_execute_internal( HThread* ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_TRYCATCH_HXX_INCLUDED */

