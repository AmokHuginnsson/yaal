/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hsearchablecontrol.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
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

namespace yaal
{

namespace hconsole
{

/*! \brief Provision H*Control classes with search capabilities.
 */
class HSearchableControl : public virtual HControl
	{
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

