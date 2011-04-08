/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpointer.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HPOINTER_HXX_INCLUDED
#define YAAL_HCORE_HPOINTER_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/trait.hxx"
#include "hcore/algorithm.hxx"

namespace yaal
{

namespace hcore
{

/* Due to extream vitality of this struct,
 * none of the methods are guarded. */

/*! \brief Types of reference counters.
 */
struct REFERENCE_COUNTER_TYPE
	{
	static int const STRICT = 0;
	static int const WEAK = 1;
	};

template<typename tType>
class HPointerFromThisInterface;

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
class HPointer;

/*! \brief Pointer life time tracker access policy.
 */
template<typename tType, typename pointer_type_t>
struct HPointerStrict
	{
	static tType* raw( tType* );
	static typename trait::make_reference<tType>::type object_at( tType*, int );
	template<typename deleter_t>
	static void delete_pointee( deleter_t const& );
	template<typename ptr_t>
	static void initialize_from_this( HPointerFromThisInterface<tType>*, ptr_t const& );
	template<typename ptr_t>
	static void initialize_from_this( void*, ptr_t const& );
	static void inc_reference_counter( int* );
	static void dec_reference_counter( int* );
	};

/*! \brief Pointer's observer access policy.
 */
template<typename tType, typename pointer_type_t>
struct HPointerWeak
	{
	template<typename deleter_t>
	inline static void delete_pointee( deleter_t const& ){}
	static void inc_reference_counter( int* );
	static void dec_reference_counter( int* );
	};

/*! \brief Pointer to scalar deletion policy.
 */
template<typename tType>
struct HPointerScalar
	{
	template<typename real_t>
	static void delete_pointee( tType* );
	static tType* raw( tType* );
	};

/*! \brief Pointer to array deletion policy.
 */
template<typename tType>
struct HPointerArray
	{
	template<typename real_t>
	static void delete_pointee( tType* );
	static tType& object_at( tType*, int );
	};

struct pointer_helper;

/*! \brief Smart pointer, reference counting implementation.
 *
 * \tparam tType - object type which life time will be guarded.
 * \tparam pointer_type_t - select pointer type, one of HPointerScalar or HPointerArray.
 * \tparam access_type_t - defines pointer kind, one of HPointerStrict or HPointerWeak.
 */
template<typename tType, template<typename>class pointer_type_t = HPointerScalar,
				 template<typename, typename>class access_type_t = HPointerStrict,
				 typename deleter_t = void ( * )( tType* )>
class HPointer
	{
	class HShared
		{
		int _referenceCounter[ 2 ];
		deleter_t DELETER;
		tType* _object;
		HShared( deleter_t const& deleter_, tType* object_ ) : _referenceCounter(), DELETER( deleter_ ), _object( object_ )
			{
			_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] = 0;
			_referenceCounter[ REFERENCE_COUNTER_TYPE::WEAK ] = 0;
			}
		void do_delete( void )
			{
			DELETER( _object );
			_object = NULL;
			}
		friend struct pointer_helper;
		friend class HPointer;
		template<typename T1, typename T2>
		friend struct HPointerStrict;
		};
	/*
	 * WARNING!
	 *
	 * Pointer to guarded object must be part of each
	 * smart-pointer instance so smart-pointer instance
	 * type information is preserved.
	 *
	 * It is necessary for proper resolving vtable
	 * bases in up/down casting of  smart-pointers 
	 * of objects of classes from multiple-inheritance
	 * (virtual or non-virtual) hierarchies.
	 */
	/*
	 * WARNING 2!
	 *
	 * Shared struct (a reference counters) must be initialized
	 * first in HPointer constructor. In case of memory allocation
	 * failure an object pointee must still be NULL.
	 */
	HShared* _shared;
	tType* _object;
public:
	typedef tType value_type;
	typedef typename trait::make_reference<tType>::type reference;
	typedef typename trait::make_reference<tType const>::type const_reference;
	typedef pointer_type_t<tType> pointer_type;
	HPointer( void );
	template<typename real_t>
	explicit HPointer( real_t* const );
	template<typename real_t>
	explicit HPointer( real_t* const, deleter_t );
	virtual ~HPointer( void );
	HPointer( HPointer const& );
	template<typename alien_t, template<typename, typename>class alien_access_t, typename alien_deleter_t>
	HPointer( HPointer<alien_t, pointer_type_t, alien_access_t, alien_deleter_t> const& );
	HPointer& operator = ( HPointer const& );
	template<typename alien_t, template<typename, typename>class alien_access_t, typename alien_deleter_t>
	HPointer& operator = ( HPointer<alien_t, pointer_type_t, alien_access_t, alien_deleter_t> const& );
	const_reference operator* ( void ) const;
	reference operator* ( void );
	const_reference operator[] ( int ) const;
	reference operator[] ( int );
	template<typename alien_t, template<typename, typename>class alien_access_t, typename alien_deleter_t>
	bool operator == ( HPointer<alien_t, pointer_type_t, alien_access_t, alien_deleter_t> const& ) const;
	template<typename alien_t>
	bool operator == ( alien_t const* const ) const;
	template<typename alien_t, template<typename, typename>class alien_access_t, typename alien_deleter_t>
	bool operator != ( HPointer<alien_t, pointer_type_t, alien_access_t, alien_deleter_t> const& ) const;
	template<typename alien_t>
	bool operator != ( alien_t const* const ) const;
	tType const* operator->( void ) const;
	tType* operator->( void );
	tType const* raw( void ) const;
	tType* raw( void );
	tType const* get( void ) const;
	tType* get( void );
	bool operator! ( void ) const;
	void swap( HPointer& );
	void reset( void );
private:
	bool release( void ) throw();
	template<typename alien_t, template<typename, typename> class alien_access_t, typename alien_deleter_t>
	void acquire( HPointer<alien_t, pointer_type_t, alien_access_t, alien_deleter_t> const& );
	void assign( tType*&, tType* );
	template<typename alien_t>
	void assign( tType*&, alien_t* );
	friend struct pointer_helper;
	};

/*! \brief Interface for concept of getting smart pointer from `this'.
 */
template<typename tType>
class HPointerFromThisInterface
	{
protected:
	typedef HPointer<tType, HPointerScalar, HPointerStrict> ptr_t;
	typedef HPointer<tType, HPointerScalar, HPointerWeak> weak_t;
	weak_t _selfObserver;
public:
	HPointerFromThisInterface( void );
	virtual ~HPointerFromThisInterface( void ){}
	ptr_t get_pointer( void );
	ptr_t const get_pointer( void ) const;
protected:
	void initialize_observer( ptr_t const& );
	friend struct HPointerStrict<tType, HPointerScalar<tType> >;
	};

template<typename tType, typename pointer_type_t>
template<typename deleter_t>
void HPointerStrict<tType, pointer_type_t>::delete_pointee( deleter_t const& deleter_ )
	{
	deleter_->do_delete();
	}

template<typename tType, typename pointer_type_t>
typename trait::make_reference<tType>::type HPointerStrict<tType, pointer_type_t>::object_at( tType* pointer_, int index_ )
	{
	return ( pointer_type_t::object_at( pointer_[ index_ ], index_ ) );
	}

template<typename tType, typename pointer_type_t>
tType* HPointerStrict<tType, pointer_type_t>::raw( tType* pointer_ )
	{
	return ( pointer_type_t::raw( pointer_ ) );
	}

template<typename tType, typename pointer_type_t>
void HPointerStrict<tType, pointer_type_t>::inc_reference_counter( int* referenceCounter_ )
	{
	++ referenceCounter_[ REFERENCE_COUNTER_TYPE::STRICT ];
	++ referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ];
	}

template<typename tType, typename pointer_type_t>
void HPointerWeak<tType, pointer_type_t>::inc_reference_counter( int* referenceCounter_ )
	{
	++ referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ];
	}

template<typename tType, typename pointer_type_t>
void HPointerStrict<tType, pointer_type_t>::dec_reference_counter( int* referenceCounter_ )
	{
	M_ASSERT( referenceCounter_[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 );
	M_ASSERT( referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ] > 0 );
	-- referenceCounter_[ REFERENCE_COUNTER_TYPE::STRICT ];
	-- referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ];
	}

template<typename tType, typename pointer_type_t>
void HPointerWeak<tType, pointer_type_t>::dec_reference_counter( int* referenceCounter_ )
	{
	M_ASSERT( referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ] > 0 );
	-- referenceCounter_[ REFERENCE_COUNTER_TYPE::WEAK ];
	}

template<typename tType>
template<typename real_t>
void HPointerScalar<tType>::delete_pointee( tType* pointer_ )
	{
	M_SAFE( delete static_cast<real_t*>( pointer_ ) );
	}

template<typename tType>
template<typename real_t>
void HPointerArray<tType>::delete_pointee( tType* pointer_ )
	{
	M_SAFE( delete [] static_cast<real_t*>( pointer_ ) );
	}

template<typename tType>
tType& HPointerArray<tType>::object_at( tType* pointer_, int index_ )
	{
	return ( pointer_[ index_ ] );
	}

template<typename tType>
tType* HPointerScalar<tType>::raw( tType* pointer_ )
	{
	return ( pointer_ );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
HPointer<tType, pointer_type_t, access_type_t, deleter_t>::HPointer( void )
	: _shared( NULL ), _object( NULL )
	{
	return;
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
template<typename real_t>
HPointer<tType, pointer_type_t, access_type_t, deleter_t>::HPointer( real_t* const pointer_ )
	: _shared( pointer_ ? new HShared( &pointer_type::template delete_pointee<real_t>, pointer_ ) : NULL ), _object( pointer_ )
	{
	M_ASSERT( pointer_ );
	_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] = 1;
	_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::WEAK ] = 1;
	access_type_t<tType, pointer_type_t<tType> >::initialize_from_this( pointer_, *this );
	return;
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
template<typename real_t>
HPointer<tType, pointer_type_t, access_type_t, deleter_t>::HPointer( real_t* const pointer_, deleter_t deleter_ )
	: _shared( pointer_ ? new HShared( deleter_, pointer_ ) : NULL ), _object( pointer_ )
	{
	M_ASSERT( pointer_ );
	_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] = 1;
	_shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::WEAK ] = 1;
	access_type_t<tType, pointer_type_t<tType> >::initialize_from_this( pointer_, *this );
	return;
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
HPointer<tType, pointer_type_t, access_type_t, deleter_t>::~HPointer( void )
	{
	reset();
	return;
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
HPointer<tType, pointer_type_t, access_type_t, deleter_t>::HPointer( HPointer<tType, pointer_type_t, access_type_t, deleter_t> const& pointer_ )
	: _shared( NULL ), _object( NULL )
	{
	acquire( pointer_ );
	return;
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
template<typename alien_t, template<typename, typename>class alien_access_t, typename alien_deleter_t>
HPointer<tType, pointer_type_t, access_type_t, deleter_t>::HPointer( HPointer<alien_t, pointer_type_t, alien_access_t, alien_deleter_t> const& pointer_ )
	: _shared( NULL ), _object( NULL )
	{
	acquire( pointer_ );
	return;
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
HPointer<tType, pointer_type_t, access_type_t, deleter_t>& HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator = ( HPointer<tType, pointer_type_t, access_type_t, deleter_t> const& pointer_ )
	{
	acquire( pointer_ );
	return ( *this );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
template<typename alien_t, template<typename, typename>class alien_access_t, typename alien_deleter_t>
HPointer<tType, pointer_type_t, access_type_t, deleter_t>& HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator = ( HPointer<alien_t, pointer_type_t, alien_access_t, alien_deleter_t> const& pointer_ )
	{
	acquire( pointer_ );
	return ( *this );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
template<typename alien_t, template<typename, typename>class alien_access_t, typename alien_deleter_t>
void HPointer<tType, pointer_type_t, access_type_t, deleter_t>::acquire( HPointer<alien_t, pointer_type_t, alien_access_t, alien_deleter_t> const& from )
	{
	HPointer const& alien = reinterpret_cast<HPointer const&>( from );
	if ( ( &alien != this ) && ( _shared != alien._shared ) )
		{
		M_ASSERT( ( ! ( _shared && alien._shared ) )
				|| ( ( _shared && alien._shared )
					&& ( _object != reinterpret_cast<alien_t*>( alien._object ) ) ) );
		if ( _shared )
			release();
		if ( alien._shared && ( alien._shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) )
			{
			access_type_t<tType, pointer_type_t<tType> >::inc_reference_counter( alien._shared->_referenceCounter );
			_shared = alien._shared;
			assign( _object, reinterpret_cast<alien_t*>( alien._object ) );
			}
		else
			{
			_shared = NULL;
			_object = NULL;
			}
		}
	return;
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
template<typename alien_t>
void HPointer<tType, pointer_type_t, access_type_t, deleter_t>::assign( tType*& to, alien_t* from )
	{
	to = dynamic_cast<tType*>( from );
	return;
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
void HPointer<tType, pointer_type_t, access_type_t, deleter_t>::assign( tType*& to, tType* from )
	{
	to = from;
	return;
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
void HPointer<tType, pointer_type_t, access_type_t, deleter_t>::swap( HPointer& p )
	{
	if ( &p != this )
		{
		using yaal::swap;
		swap( _shared, p._shared );
		swap( _object, p._object );
		}
	return;
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
bool HPointer<tType, pointer_type_t, access_type_t, deleter_t>::release( void ) throw()
	{
	M_ASSERT( _shared );
	if ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] == 1 )
		access_type_t<tType, pointer_type_t<tType> >::delete_pointee( _shared );
	access_type_t<tType, pointer_type_t<tType> >::dec_reference_counter( _shared->_referenceCounter );
	if ( ! _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::WEAK ] )
		{
		delete _shared;
		_shared = NULL;
		}
	return ( ! ( _shared && _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] ) );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
void HPointer<tType, pointer_type_t, access_type_t, deleter_t>::reset( void )
	{
	_shared && release();
	_shared = NULL;
	_object = NULL;
	return;
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
typename HPointer<tType, pointer_type_t, access_type_t, deleter_t>::const_reference HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator* ( void ) const
	{
	M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
	return ( *_object );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
typename HPointer<tType, pointer_type_t, access_type_t, deleter_t>::reference HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator* ( void )
	{
	M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
	return ( *_object );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
typename HPointer<tType, pointer_type_t, access_type_t, deleter_t>::const_reference HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator[] ( int index_ ) const
	{
	M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
	M_ASSERT( index_ >= 0 );
	return ( access_type_t<tType, pointer_type_t<tType> >::object_at( _object, index_ ) );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
typename HPointer<tType, pointer_type_t, access_type_t, deleter_t>::reference HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator[] ( int index_ )
	{
	M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
	M_ASSERT( index_ >= 0 );
	return ( access_type_t<tType, pointer_type_t<tType> >::object_at( _object, index_ ) );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
template<typename alien_t, template<typename, typename>class alien_access_t, typename alien_deleter_t>
bool HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator == ( HPointer<alien_t, pointer_type_t, alien_access_t, alien_deleter_t> const & pointer_ ) const
	{
	HPointer const* alien = reinterpret_cast<HPointer const *>( &pointer_ );
	return ( _object == reinterpret_cast<alien_t*>( alien->_object ) );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
template<typename alien_t>
bool HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator == ( alien_t const* const pointer_ ) const
	{
	return ( _object == pointer_ );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
template<typename alien_t, template<typename, typename>class alien_access_t, typename alien_deleter_t>
bool HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator != ( HPointer<alien_t, pointer_type_t, alien_access_t, alien_deleter_t>const & pointer_ ) const
	{
	return ( ! operator == ( pointer_ ) );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
template<typename alien_t>
bool HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator != ( alien_t const* const pointer_ ) const
	{
	return ( ! operator == ( pointer_ ) );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t, typename alien_t>
bool operator == ( alien_t const* const pointer_, HPointer<tType, pointer_type_t, access_type_t, deleter_t> const& smartPointer_ )
	{
	return ( smartPointer_ == pointer_ );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t, typename alien_t>
bool operator != ( alien_t const* const pointer_, HPointer<tType, pointer_type_t, access_type_t, deleter_t> const& smartPointer_ )
	{
	return ( smartPointer_ != pointer_ );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
tType const* HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator->( void ) const
	{
	M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
	return ( access_type_t<tType, pointer_type_t<tType> >::raw( _object ) );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
tType* HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator->( void )
	{
	M_ASSERT( _shared && ( _shared->_referenceCounter[ REFERENCE_COUNTER_TYPE::STRICT ] > 0 ) );
	return ( access_type_t<tType, pointer_type_t<tType> >::raw( _object ) );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
tType const* HPointer<tType, pointer_type_t, access_type_t, deleter_t>::raw( void ) const
	{
	return ( _object );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
tType* HPointer<tType, pointer_type_t, access_type_t, deleter_t>::raw( void )
	{
	return ( _object );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
tType const* HPointer<tType, pointer_type_t, access_type_t, deleter_t>::get( void ) const
	{
	return ( _object );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
tType* HPointer<tType, pointer_type_t, access_type_t, deleter_t>::get( void )
	{
	return ( _object );
	}

template<typename tType, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
bool HPointer<tType, pointer_type_t, access_type_t, deleter_t>::operator ! ( void ) const
	{
	return ( ! _object );
	}

template<typename tType, typename pointer_type_t>
template<typename ptr_t>
void HPointerStrict<tType, pointer_type_t>::initialize_from_this( HPointerFromThisInterface<tType>* obj,
		ptr_t const& ptr )
	{
	obj->initialize_observer( ptr );
	return;
	}

template<typename tType, typename pointer_type_t>
template<typename ptr_t>
void HPointerStrict<tType, pointer_type_t>::initialize_from_this( void*,
		ptr_t const& )
	{
	return;
	}

template<typename tType>
HPointerFromThisInterface<tType>::HPointerFromThisInterface( void ) : _selfObserver()
	{
	return;
	}

template<typename tType>
void HPointerFromThisInterface<tType>::initialize_observer( ptr_t const& firstOwner_ )
	{
	_selfObserver = firstOwner_;
	return;
	}

template<typename tType>
typename HPointerFromThisInterface<tType>::ptr_t HPointerFromThisInterface<tType>::get_pointer( void )
	{
	return ( _selfObserver );
	}

template<typename tType>
typename HPointerFromThisInterface<tType>::ptr_t const HPointerFromThisInterface<tType>::get_pointer( void ) const
	{
	return ( _selfObserver );
	}

struct pointer_helper
	{
	template<typename to_t, typename from_t, template<typename>class pointer_type_t,
					 template<typename, typename>class access_type_t, typename deleter_t>
	static typename yaal::hcore::HPointer<to_t, pointer_type_t, access_type_t, deleter_t> do_static_cast( HPointer<from_t, pointer_type_t, access_type_t, deleter_t> from_ )
		{
		HPointer<to_t, pointer_type_t, access_type_t, deleter_t> to;
		if ( from_._object )
			{
			to._shared = reinterpret_cast<typename HPointer<to_t, pointer_type_t, access_type_t, deleter_t>::HShared*>( from_._shared );
			to._object = static_cast<to_t*>( from_._object );
			access_type_t<to_t, pointer_type_t<to_t> >::inc_reference_counter( to._shared->_referenceCounter );
			}
		return ( to );
		}

	template<typename to_t, typename from_t, template<typename>class pointer_type_t,
					 template<typename, typename>class access_type_t, typename deleter_t>
	static typename yaal::hcore::HPointer<to_t, pointer_type_t, access_type_t, deleter_t> do_dynamic_cast( HPointer<from_t, pointer_type_t, access_type_t, deleter_t> from_ )
		{
		HPointer<to_t, pointer_type_t, access_type_t, deleter_t> to;
		if ( dynamic_cast<to_t*>( from_._object ) )
			{
			to._shared = reinterpret_cast<typename HPointer<to_t, pointer_type_t, access_type_t, deleter_t>::HShared*>( from_._shared );
			to._object = static_cast<to_t*>( from_._object );
			access_type_t<to_t, pointer_type_t<to_t> >::inc_reference_counter( to._shared->_referenceCounter );
			}
		return ( to );
		}

	};

template<typename to_t, typename from_t, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
typename yaal::hcore::HPointer<to_t, pointer_type_t, access_type_t, deleter_t> pointer_static_cast( HPointer<from_t, pointer_type_t, access_type_t, deleter_t> from_ )
	{
	return ( pointer_helper::do_static_cast<to_t>( from_ ) );
	}

template<typename to_t, typename from_t, template<typename>class pointer_type_t,
				 template<typename, typename>class access_type_t, typename deleter_t>
typename yaal::hcore::HPointer<to_t, pointer_type_t, access_type_t, deleter_t> pointer_dynamic_cast( HPointer<from_t, pointer_type_t, access_type_t, deleter_t> from_ )
	{
	return ( pointer_helper::do_dynamic_cast<to_t>( from_ ) );
	}

}

template<typename tType, template<typename>class pointer_type_t,
	template<typename, typename>class access_type_t, typename deleter_t>
inline void swap( yaal::hcore::HPointer<tType, pointer_type_t, access_type_t, deleter_t>& a, yaal::hcore::HPointer<tType, pointer_type_t, access_type_t, deleter_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HPOINTER_HXX_INCLUDED */

