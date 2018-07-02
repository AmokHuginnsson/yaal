/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_TRYCATCH_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_TRYCATCH_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "tools/huginn/statement.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HTryCatch : public HStatement {
public:
	typedef HTryCatch this_type;
	typedef HStatement base_type;
	class HCatch : public HStatement {
		HHuginn::identifier_id_t _type;
		HHuginn::expression_t _control;
		HHuginn::scope_t _scope;
	public:
		HCatch(
			HStatement::statement_id_t,
			HHuginn::identifier_id_t,
			HHuginn::expression_t const&,
			HHuginn::scope_t const&
		);
		void execute( HThread*, HHuginn::value_t ) const;
		HHuginn::identifier_id_t type( void ) const {
			return ( _type );
		}
	};
	typedef yaal::hcore::HArray<HCatch> catches_t;
private:
	HHuginn::scope_t _try;
	catches_t _catches;
public:
	HTryCatch( HHuginn::scope_t const&, catches_t const&, int, executing_parser::range_t );
protected:
	virtual void do_execute( HThread* ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_TRYCATCH_HXX_INCLUDED */

