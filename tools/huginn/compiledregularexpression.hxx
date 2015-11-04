/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  compiledregularexpression.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_COMPILEDREULAREXPRESSION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_COMPILEDREULAREXPRESSION_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"
#include "hcore/hresource.hxx"
#include "hcore/hregex.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HCompiledRegularExpression : public HHuginn::HObject {
public:
	typedef yaal::hcore::HResource<yaal::hcore::HRegex> regex_t;
private:
	regex_t _regex;
public:
	HCompiledRegularExpression( HHuginn::HClass const*, regex_t );
	static HHuginn::value_t match( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const&, int );
	static HHuginn::value_t groups( huginn::HThread*, HHuginn::HObject*, HHuginn::values_t const&, int );
	static HHuginn::class_t get_class( HHuginn*, HHuginn::class_t const& );
private:
	HHuginn::value_t do_match( huginn::HThread*, HHuginn::values_t const&, int );
	HHuginn::value_t do_groups( huginn::HThread*, HHuginn::values_t const&, int );
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_COMPILEDREULAREXPRESSION_HXX_INCLUDED */
