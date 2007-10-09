/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfsitem.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_TOOLS_HFSITEM_H
#define __YAAL_TOOLS_HFSITEM_H

#include "hcore/hstring.h"
#include "hcore/hchunk.h"
#include "hcore/hpointer.h"

namespace yaal
{

namespace tools
{

class HFSItem
	{
	yaal::hcore::HString f_oRoot;
	yaal::hcore::HChunk f_oDirEnt;
public:
	typedef yaal::hcore::HPointer<HFSItem, yaal::hcore::HPointerScalar, yaal::hcore::HPointerRelaxed> ptr_t;
	class HIterator;
	explicit HFSItem( yaal::hcore::HString const& );
	bool is_dir( void );
	yaal::hcore::HString get_name();
	HIterator begin( void );
	HIterator end( void );
private:
	void next( void );
	void previous( void );
	};

class HFSItem::HIterator
	{
	HFSItem::ptr_t f_oEngine;
public:
	HIterator( HIterator const& a_oIt ) : f_oEngine( a_oIt.f_oEngine ) {}
	HIterator& operator= ( HIterator const& a_oIt )
		{
		if ( &a_oIt != this )
			f_oEngine = a_oIt.f_oEngine;
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		f_oEngine->next();
		return ( *this );
		}
	HIterator const operator ++ ( int );
/*		{
		HIterator it( f_oEngine );
		f_oEngine->next();
		return ( it );
		}*/
	HIterator& operator -- ( void )
		{
		f_oEngine->previous();
		return ( *this );
		}
	HIterator const operator -- ( int );
/*		{
		HIterator it( f_oEngine );
		f_oEngine->previous();
		return ( it );
		}*/
	HFSItem const& operator * ( void )
		{	return ( f_oEngine.operator*() );	}
	HFSItem const* const operator -> ( void )
		{ return ( &f_oEngine.operator*() );	}
	bool operator == ( HIterator const& it ) const
		{ return ( f_oEngine == it.f_oEngine ); }
	bool operator != ( HIterator const& it ) const
		{ return ( f_oEngine != it.f_oEngine ); }
private:
	friend class HFSItem;
	explicit HIterator( yaal::hcore::HString const& it );// : f_oEngine( it ) {};
	};

}

}

#endif /* not __YAAL_TOOLS_HFSITEM_H */

