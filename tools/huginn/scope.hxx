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

class HVirtualScope : public HStatement {
public:
	typedef HVirtualScope this_type;
	typedef HStatement base_type;
private:
	int _variableCount;
public:
	HVirtualScope( HHuginn::statement_id_t, int, executing_parser::range_t );
	virtual ~HVirtualScope( void ) {
		return;
	}
	void execute_internal( HThread* thread_ ) const {
		do_execute_internal( thread_ );
		thread_->current_frame()->pop_local_variables( _variableCount );
	}
	void set_variable_count( int );
protected:
	virtual void do_execute( HThread* ) const override;
	virtual void do_execute_internal( HThread* ) const = 0;
private:
	HVirtualScope( HVirtualScope const& ) = delete;
	HVirtualScope( HVirtualScope&& ) = delete;
	HVirtualScope& operator = ( HVirtualScope const& ) = delete;
};

class HScope : public HVirtualScope {
public:
	typedef HScope this_type;
	typedef HVirtualScope base_type;
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
	virtual void do_execute_internal( HThread* ) const override;
private:
	HScope( HScope const& ) = delete;
	HScope( HScope&& ) = delete;
	HScope& operator = ( HScope const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_SCOPE_HXX_INCLUDED */

