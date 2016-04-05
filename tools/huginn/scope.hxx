/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  scope.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_SCOPE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_SCOPE_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hcore/hpointer.hxx"
#include "tools/huginn/statement.hxx"
#include "hcore/htaggedpod.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HScope : public HStatement {
public:
	typedef HScope this_type;
	typedef HStatement base_type;
	typedef yaal::hcore::HPointer<HStatement> statement_t;
	typedef yaal::hcore::HArray<statement_t> statement_list_t;
	struct scope_tag;
	typedef yaal::hcore::HTaggedPOD<int, scope_tag> scope_id_t;
private:
	statement_list_t _statements;
	bool _inline;
	scope_id_t _id;
public:
	HScope( scope_id_t, int );
	virtual ~HScope( void ) {
		return;
	}
	int add_statement( statement_t );
	void make_inline( void );
	int statement_position_at( int ) const;
	int statement_count( void ) const {
		return ( static_cast<int>( _statements.get_size() ) );
	}
	scope_id_t id( void ) const {
		return ( _id );
	}
protected:
	virtual void do_execute( HThread* ) const override;
private:
	HScope( HScope const& ) = delete;
	HScope( HScope&& ) = delete;
	HScope& operator = ( HScope const& ) = delete;
};

extern HScope::scope_id_t const INVALID_SCOPE_IDENTIFIER;

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_SCOPE_HXX_INCLUDED */

