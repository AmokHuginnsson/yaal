/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hhashcontainer.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hhashcontainer.hxx
 * \brief Declaration of HHashContainer<>, a halper for HHash* and HMultiHash* classes.
 */

#ifndef YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED
#define YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED 1

#include "hcore/hchunk.hxx"

namespace yaal
{

namespace hcore
{

class HHashContainer
	{
	class HIterator;
private:
	class HAbstractAtom
		{
	private:
		HAbstractAtom* _next;
		HAbstractAtom( HAbstractAtom const& );
		virtual ~HAbstractAtom( void );
		HAbstractAtom& operator = ( HAbstractAtom const& );
		friend class HHashContainer;
		friend class HIterator;
		};
	template<typename tType>
	class HAtom;
	int long _prime;
	int _size;
	HChunk _buckets;
public:
	void resize( int long );
	void clear( void );
	void erase( HIterator const& );
	HIterator begin( void ) const;
	HIterator end( void ) const;
	HIterator rbegin( void ) const;
	HIterator rend( void ) const;
private:
	template<typename tType>
	bool find_atom( tType const& key_, int long& index_, HAtom<tType>*& atom_ ) const;
	};

/*! \brief Iterator for HHashContainer data structure.
 */
class HHashContainer::HIterator
	{
	HHashContainer* _owner;
	int long _index;
	HHashContainer::HAbstractAtom* _atom;
public:
	HIterator( void ) : _owner( NULL ), _index( 0 ), _atom( NULL ) {}
	HIterator( HIterator const& it_ )
		: _owner( it_._owner ), _index( it_._index ),
		_atom( it_._atom )
		{
		}
	HIterator& operator = ( HIterator const& it_ )
		{
		if ( &it_ != this )
			{
			_owner = it_._owner;
			_index = it_._index;
			_atom = it_._atom;
			}
		return ( *this );
		}
	HIterator& operator ++ ( void )
		{
		if ( _atom )
			{
			_atom = _atom->_next;
			if ( ! _atom )
				_index ++;
			}
		if ( ! _atom )
			{
			HHashContainer::HAbstractAtom** buckets( _owner->_buckets.get<HHashContainer::HAbstractAtom*>() );
			while ( ( _index < _owner->_prime ) && ! buckets[ _index ] )
				_index ++;
			if ( _index < _owner->_prime )
				_atom = buckets[ _index ];
			}
		if ( ! _atom )
			_index = _owner->_prime;
		return ( *this );
		}
	HIterator const operator ++ ( int )
		{
		HIterator it( *this );
		operator ++ ();
		return ( it );
		}
	HIterator& operator -- ( void )
		{
		return ( *this );
		}
	HIterator const operator -- ( int )
		{
		HIterator it( *this );
		operator -- ();
		return ( it );
		}
	template<typename tType>
	tType& operator* ( void )
		{
		M_ASSERT( _atom );
		return ( static_cast<HHashContainer::HAtom<tType>*>( _atom )->_value );
		}
	template<typename tType>
	tType const& operator* ( void ) const
		{
		M_ASSERT( _atom );
		return ( static_cast<HHashContainer::HAtom<tType> const*>( _atom )->_value );
		}
	bool operator == ( HIterator const& it ) const
		{
		M_ASSERT( _owner == it._owner );
		return ( ( _index == it._index ) && ( _atom == it._atom ) );
		}
	bool operator != ( HIterator const& it ) const
		{ return ( ! operator == ( it ) ); }
private:
	friend class HHashContainer;
	explicit HIterator( HHashContainer* owner_, int long index_, HHashContainer::HAbstractAtom* atom_ )
		: _owner( owner_ ), _index( index_ ), _atom( atom_ ) {};
	};

template<typename tType>
bool HHashContainer::find_atom( tType const& key_, int long& index_, HAtom<tType>*& atom_ ) const
	{
	M_PROLOG
	index_ = abs( _hasher( key_ ) ) % _prime;
	atom_ = _buckets.get<HAtom<tType>*>()[ index_ ];
	while ( atom_ && ( atom_->_value.first != key_ ) )
		atom_ = atom_->_next;
	return ( atom_ ? true : false );
	M_EPILOG
	}

}

}

#endif /* not YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED */

