/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hsearchablecontrol.hxx - this file is integral part of `yaal' project.

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
/*! \file hconsole/hsearchablecontrol.hxx
 * \brief Declaration of HSearchableControl class.
 */

#ifndef YAAL_HCONSOLE_HSEARCHABLECONTROL_HXX_INCLUDED
#define YAAL_HCONSOLE_HSEARCHABLECONTROL_HXX_INCLUDED 1

#include "hcore/hpattern.hxx"
#include "hconsole/hcontrol.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Provision H*Control classes with search capabilities.
 */
class HSearchableControl : public virtual HControl {
public:
	typedef HSearchableControl this_type;
	typedef HControl base_type;
protected:
	bool _searchable;		/*!< are items searchable */
	bool _searchActived;	/*!< should we highlight last search */
	bool _filtered;			/*!< is content of list filtered thru pattern */
	bool _backwards;			/*!< last search direction */
	hcore::HPattern	_pattern;	/*!< used for searching */
public:
	HSearchableControl ( bool );
	virtual ~HSearchableControl ( void );
	void search( hcore::HString const &, bool );
protected:
	void highlight( int, int, int, bool );
	virtual void go_to_match( void ) = 0;
	virtual void go_to_match_previous( void ) = 0;
};

typedef yaal::hcore::HExceptionT<HSearchableControl, HControlException> HSearchableControlException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HSEARCHABLECONTROL_HXX_INCLUDED */

