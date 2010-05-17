/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hhashcontainer.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hhashcontainer.hxx"

namespace yaal
{

namespace hcore
{

int long HHashContainer::get_size( void ) const
	{
	M_PROLOG
	return ( _size );
	M_EPILOG
	}

int long HHashContainer::size( void ) const
	{
	M_PROLOG
	return ( _size );
	M_EPILOG
	}

bool HHashContainer::is_empty( void ) const
	{
	M_PROLOG
	return ( ! _size );
	M_EPILOG
	}

bool HHashContainer::empty( void ) const
	{
	M_PROLOG
	return ( ! _size );
	M_EPILOG
	}

void HHashContainer::clear( void )
	{
	M_PROLOG
	
	HAbstractAtom** buckets( _buckets.get<HAbstractAtom*>() );
	for ( int long i = 0; i < _prime; i ++ )
		{
		HAbstractAtom* atom( buckets[ i ] );
		while ( atom )
			{
			HAbstractAtom* del( atom );
			atom = atom->_next;
			delete del;
			}
		buckets[ i ] = NULL;
		}
	_size = 0;
	return;
	M_EPILOG
	}

void HHashContainer::erase( HIterator const& it )
	{
	M_PROLOG
	HAbstractAtom* atom( _buckets.get<HAbstractAtom*>()[ it._index ] );
	HAbstractAtom* ancestor( NULL );
	M_ASSERT( atom );
	while ( atom != it._atom )
		ancestor = atom, atom = atom->_next;
	if ( atom )
		{
		if ( ancestor )
			ancestor->_next = atom->_next;
		else
			_buckets.get<HAbstractAtom*>()[ it._index ] = NULL;
		delete atom;
		-- _size;
		}
	return;
	M_EPILOG
	}

HHashContainer::HIterator HHashContainer::begin( void ) const
	{
	M_PROLOG
	HIterator it( this, 0, NULL );
	return ( ++ it );
	M_EPILOG
	}

HHashContainer::HIterator HHashContainer::end( void ) const
	{
	M_PROLOG
	return ( HIterator( this, _prime, NULL ) );
	M_EPILOG
	}

}

}

