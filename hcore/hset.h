/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hset.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HSET_H
#define __YAAL_HCORE_HSET_H

#line 31

#define D_VCSID_HSET_H "$Id$"

#include "hcore/hsbbstree.h"

namespace yaal
{

namespace hcore
{

template < typename tType >
class HSet
	{
public:
	class HIterator
		{
		HSBBSTree::HIterator f_oEngine;
	public:
		explicit HIterator ( HSBBSTree::HIterator const & it ) : f_oEngine ( it ){};
		HIterator & operator ++ ( void )
			{
			++ f_oEngine;
			return ( * this );
			}
		HIterator const operator ++ ( int )
			{
			HIterator it ( f_oEngine );
			++ f_oEngine;
			return ( it );
			}
		HIterator & operator -- ( void )
			{
			-- f_oEngine;
			return ( * this );
			}
		HIterator const operator -- ( int )
			{
			HIterator it ( f_oEngine );
			-- f_oEngine;
			return ( it );
			}
		tType const & operator * ( void )
			{	return ( f_oEngine.operator*<tType> ( ) );	}
		tType const * const operator -> ( void )
			{ return ( & f_oEngine.operator*<tType> ( ) );	}
		bool operator == ( HIterator const & it ) const
			{ return ( f_oEngine == it.f_oEngine ); }
		bool operator != ( HIterator const & it ) const
			{ return ( f_oEngine != it.f_oEngine ); }
		};
private:
	HSBBSTree f_oEngine;
public:
	HSet ( void ) : f_oEngine ( ) {};
	size_t size ( void ) const
		{ return ( f_oEngine.size ( ) );	}
	bool empty ( void ) const
		{ return ( f_oEngine.empty ( ) );	}
	void insert ( tType const & e )
		{	f_oEngine.insert ( e );	}
	void remove ( tType const & e )
		{	f_oEngine.remove ( e );	}
	HIterator find ( tType const & e ) const
		{ return ( f_oEngine.find ( e ) ); }
	HIterator begin ( void ) const
		{ return ( HIterator ( f_oEngine.begin ( ) ) ); }
	HIterator end ( void ) const
		{ return ( HIterator ( f_oEngine.end ( ) ) ); }
	HIterator rbegin ( void ) const
		{ return ( HIterator ( f_oEngine.rbegin ( ) ) ); }
	HIterator rend ( void ) const
		{ return ( HIterator ( f_oEngine.rend ( ) ) ); }
	};

}

}

#endif /* not __YAAL_HCORE_HSET_H */

