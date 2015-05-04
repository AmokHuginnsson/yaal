/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  if.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_IF_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_IF_HXX_INCLUDED 1

#include "compiler.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HIf : public HStatement {
public:
	typedef HIf this_type;
	typedef HStatement base_type;
	typedef OCompiler::OCompilationFrame::contexts_t if_clauses_t;
private:
	if_clauses_t _ifClauses;
	HHuginn::scope_t _elseClause;
public:
	HIf( if_clauses_t const&, HHuginn::scope_t const& );
protected:
	virtual void do_execute( HThread* ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_IF_HXX_INCLUDED */
