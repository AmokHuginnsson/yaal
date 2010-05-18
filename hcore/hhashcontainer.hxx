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

#include "hcore/trait.hxx"
#include "hcore/hchunk.hxx"

namespace yaal
{

namespace hcore
{

extern int long const* const _primes_;

class HHashContainer : private trait::HNonCopyable
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
		virtual HAbstractAtom* clone( void ) const = 0;
		friend class HHashContainer;
		friend class HIterator;
		};
	template<typename tType>
	class HAtom : public HAbstractAtom
		{
		tType _value;
		HAtom( tType const& value_ ) : HAbstractAtom(), _value( value_ ) {}
		virtual HAbstractAtom* clone( void ) const
			{ return ( new HAtom( _value ) ); }
		friend class HHashContainer;
		friend class HIterator;
		};
	int long _prime;
	int _size;
	HChunk _buckets;
public:
	int long get_size( void ) const;
	bool is_empty( void ) const;
	void clear( void );
	void erase( HIterator const& );
	HIterator begin( void ) const;
	HIterator end( void ) const;
	HIterator rbegin( void ) const;
	HIterator rend( void ) const;
	template<typename tType>
	void resize( int long, tType const& );
private:
	void copy_from( HHashContainer const& );
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

template<typename tType>
void HHashContainer::resize( int long size_, tType const& hasher_ )
	{
	M_PROLOG
	if ( size_ < 1 )
		M_THROW ( "bad new container size", size_ );
	if ( size_ > _size )
		{
		int n = 0;
		while ( size_ )
			{
			size_ >>= 1;
			n ++;
			}
		int long prime( _primes_[ n - 1 ] );
		HChunk buckets( chunk_size<HAtom<typename tType::value_type>*>( prime ), HChunk::STRATEGY::GEOMETRIC );
		M_ASSERT( ( buckets.size() / static_cast<int long>( sizeof ( HAtom<typename tType::value_type>* ) ) ) >= prime );
		HAtom<typename tType::value_type>** oldBuckets( _buckets.get<HAtom<typename tType::value_type>*>() );
		HAtom<typename tType::value_type>** newBuckets( buckets.get<HAtom<typename tType::value_type>*>() );
		for ( int long i( 0 ); i < _prime; ++ i )
			{
			HAtom<typename tType::value_type>* a( oldBuckets[ i ] );
			while ( a )
				{
				HAtom<typename tType::value_type>* atom( a );
				a = a->_next;
				int long newHash( hasher_( atom->_value ) % prime );
				atom->_next = newBuckets[ newHash ];
				newBuckets[ newHash ] = atom;
				}
			}
		_buckets.swap( buckets );
		_prime = prime;
		}
	return;
	M_EPILOG
	}

}

}

#endif /* not YAAL_HCORE_HHASHCONTAINER_HXX_INCLUDED */

