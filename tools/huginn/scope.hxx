/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_SCOPE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_SCOPE_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hpointer.hxx"
#include "tools/huginn/statement.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HScope : public HStatement {
public:
	typedef HScope this_type;
	typedef HStatement base_type;
	typedef yaal::hcore::HPointer<HStatement> statement_t;
	typedef yaal::hcore::HArray<statement_t> statement_list_t;
private:
	statement_list_t _statements;
	bool _inline;
public:
	HScope( HHuginn::statement_id_t, int, executing_parser::range_t );
	virtual ~HScope( void ) {
		return;
	}
	int add_statement( statement_t );
	void remove_statement( int );
	void make_inline( void );
	int statement_position_at( int ) const;
	int statement_count( void ) const {
		return ( static_cast<int>( _statements.get_size() ) );
	}
protected:
	virtual void do_execute( HThread* ) const override;
private:
	HScope( HScope const& ) = delete;
	HScope( HScope&& ) = delete;
	HScope& operator = ( HScope const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_SCOPE_HXX_INCLUDED */

