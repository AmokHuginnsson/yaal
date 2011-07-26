/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hresource.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/hresource.hxx
 * \brief Declaration of HResource<> class.
 *
 * Scoped reasource guardian with definable resource free freeing function.
 */

#ifndef YAAL_HCORE_HRESOURCE_HXX_INCLUDED
#define YAAL_HCORE_HRESOURCE_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/algorithm.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Resource lifetime tracking helper.
 */
struct HResourceAllocatedBy
	{
	template<typename T>
	struct by_pointer
		{
		typedef typename trait::strip_const<T>::type* hold_t;
		typedef T const* const_hold_t;
		typedef typename trait::make_reference<typename trait::strip_const<T>::type>::type ref_t;
		typedef typename trait::make_reference<T const>::type const_ref_t;
		static ref_t ref( hold_t val )
			{ return ( *val ); }
		static const_ref_t ref( const_hold_t val )
			{ return ( *val ); }
		static T* raw( hold_t val )
			{ return ( val ); }
		static T const* raw( const_hold_t val )
			{ return ( val ); }
		};
	template<typename T>
	struct by_value
		{
		typedef T hold_t;
		typedef T const& const_hold_t;
		typedef T& ref_t;
		typedef T const& const_ref_t;
		static T ref( T& val )
			{ return ( val ); }
		static T ref( T const& val )
			{ return ( val ); }
		static T raw( T& val )
			{ return ( val ); }
		static T raw( T const& val )
			{ return ( val ); }
		};
	};
template<typename T, typename real_t>
struct HResourceReleaseBy
	{
	static void delete_obj( T p )
		{
		M_SAFE( delete static_cast<real_t>( p ) );
		}
	static void delete_array( T p )
		{
		M_SAFE( delete [] static_cast<real_t>( p ) );
		}
	};
struct HResourceReleaseWhen
	{
	struct always
		{
		static bool is_allocated( void* )
			{ return ( true ); }
		};
	struct non_null
		{
		static bool is_allocated( void* value )
			{ return ( value != NULL ); }
		template<typename T>
		static void reset( T*& p )
			{
			p = NULL;
			}
		};
	template<int than>
	struct greater
		{
		static bool is_allocated( int long value )
			{ return ( value > than ); }
		template<typename T>
		static void reset( T& p )
			{
			p = than;
			}
		};
	template<int than>
	struct greater_or_eq
		{
		static bool is_allocated( int long value )
			{ return ( value >= than ); }
		template<typename T>
		static void reset( T& p )
			{
			p = than - 1;
			}
		};
	};

/*! \brief Provide reference semantics for HResource<>.
 *
 */
template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
struct HResourceRef
	{
	typedef typename hold_by_t<type_t>::hold_t hold_t;
	hold_t _hold;
	free_t _free;
	explicit HResourceRef( hold_t hold_, free_t free_ ) : _hold( hold_ ), _free( free_ ) {}
	};

/*! \brief Raw resource life time tracker.
 */
template<typename type_t, typename free_t = void (*)( type_t* ), template<typename>class hold_by_t = HResourceAllocatedBy::by_pointer, typename allocated_t = HResourceReleaseWhen::non_null>
class HResource
	{
	typedef typename hold_by_t<type_t>::hold_t hold_t;
	typedef typename hold_by_t<type_t>::const_hold_t const_hold_t;
	typedef typename hold_by_t<type_t>::ref_t ref_t;
	typedef typename hold_by_t<type_t>::const_ref_t const_ref_t;
	hold_t _resource;
	free_t _free;
public:
	typedef type_t value_type;
	HResource( void );
	template<typename real_t>
	explicit HResource( real_t );
	template<typename real_t, typename deleter_t>
	HResource( real_t, deleter_t );
	~HResource( void );
	HResource( HResource& );
	template<typename real_t>
	HResource( HResource<real_t, free_t, hold_by_t, allocated_t>& );
	HResource( HResourceRef<type_t, free_t, hold_by_t, allocated_t> );
	HResource& operator = ( HResource& );
	template<typename real_t, typename real_free_t>
	HResource& operator = ( HResource<real_t, real_free_t, hold_by_t, allocated_t>& );
	HResource& operator = ( HResourceRef<type_t, free_t, hold_by_t, allocated_t> );
	template<typename real_t>
	operator HResourceRef<real_t, free_t, hold_by_t, allocated_t>( void );
	template<typename real_t>
	operator HResource<real_t, free_t, hold_by_t, allocated_t>( void );
	const_ref_t operator*( void ) const;
	ref_t operator*( void );
	const_hold_t operator->( void ) const;
	hold_t operator->( void );
	const_hold_t get( void ) const;
	hold_t get( void );
	void reset( void );
	hold_t release( void );
	void swap( HResource& other );
private:
	template<typename alien_t>
	void pass( alien_t& );
	};

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
HResource<type_t, free_t, hold_by_t, allocated_t>::HResource( void )
	: _resource(), _free()
	{
	return;
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
HResource<type_t, free_t, hold_by_t, allocated_t>::~HResource( void )
	{
	reset();
	return;
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
template<typename real_t>
HResource<type_t, free_t, hold_by_t, allocated_t>::HResource( real_t resource_ )
		: _resource( resource_ ), _free( &HResourceReleaseBy<hold_t, real_t>::delete_obj )
	{
	return;
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
template<typename real_t, typename deleter_t>
HResource<type_t, free_t, hold_by_t, allocated_t>::HResource( real_t resource_, deleter_t free_ )
		: _resource( resource_ ), _free( free_ )
	{
	return;
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
HResource<type_t, free_t, hold_by_t, allocated_t>::HResource( HResource& src_ )
	: _resource(), _free()
	{
	pass( src_ );
	return;
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
template<typename real_t>
HResource<type_t, free_t, hold_by_t, allocated_t>::HResource( HResource<real_t, free_t, hold_by_t, allocated_t>& src_ )
	: _resource(), _free()
	{
	pass( src_ );
	return;
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
HResource<type_t, free_t, hold_by_t, allocated_t>::HResource( HResourceRef<type_t, free_t, hold_by_t, allocated_t> src_ )
	: _resource( src_._hold ), _free( src_._free )
	{
	return;
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
HResource<type_t, free_t, hold_by_t, allocated_t>& HResource<type_t, free_t, hold_by_t, allocated_t>::operator = ( HResource& src_ )
	{
	if ( &src_ != this )
		pass( src_ );
	return ( *this );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
template<typename real_t, typename real_free_t>
HResource<type_t, free_t, hold_by_t, allocated_t>& HResource<type_t, free_t, hold_by_t, allocated_t>::operator = ( HResource<real_t, real_free_t, hold_by_t, allocated_t>& src_ )
	{
	if ( &reinterpret_cast<HResource&>( src_ ) != this )
		pass( src_ );
	return ( *this );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
HResource<type_t, free_t, hold_by_t, allocated_t>& HResource<type_t, free_t, hold_by_t, allocated_t>::operator = ( HResourceRef<type_t, free_t, hold_by_t, allocated_t> src_ )
	{
	if ( src_._hold != _resource )
		{
		HResource drop( *this );
		using yaal::swap;
		swap( _resource, src_._hold );
		swap( _free, src_._free );
		}
	return ( *this );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
template<typename real_t>
HResource<type_t, free_t, hold_by_t, allocated_t>::operator HResourceRef<real_t, free_t, hold_by_t, allocated_t>( void )
	{
	HResourceRef<real_t, free_t, hold_by_t, allocated_t> ref( release(), _free );
	_free = free_t();
	return ( ref );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
template<typename real_t>
HResource<type_t, free_t, hold_by_t, allocated_t>::operator HResource<real_t, free_t, hold_by_t, allocated_t>( void )
	{
	HResource<real_t, free_t, hold_by_t, allocated_t> ref( release(), _free );
	_free = free_t();
	return ( ref );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
typename HResource<type_t, free_t, hold_by_t, allocated_t>::const_ref_t HResource<type_t, free_t, hold_by_t, allocated_t>::operator*( void ) const
	{
	M_ASSERT( allocated_t::is_allocated( _resource ) );
	return ( hold_by_t<value_type>::ref( _resource ) );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
typename HResource<type_t, free_t, hold_by_t, allocated_t>::ref_t HResource<type_t, free_t, hold_by_t, allocated_t>::operator*( void )
	{
	M_ASSERT( allocated_t::is_allocated( _resource ) );
	return ( hold_by_t<value_type>::ref( _resource ) );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
typename HResource<type_t, free_t, hold_by_t, allocated_t>::const_hold_t HResource<type_t, free_t, hold_by_t, allocated_t>::operator->( void ) const
	{
	M_ASSERT( allocated_t::is_allocated( _resource ) );
	return ( hold_by_t<value_type>::raw( _resource ) );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
typename HResource<type_t, free_t, hold_by_t, allocated_t>::hold_t HResource<type_t, free_t, hold_by_t, allocated_t>::operator->( void )	
	{
	M_ASSERT( allocated_t::is_allocated( _resource ) );
	return ( hold_by_t<value_type>::raw( _resource ) );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
typename HResource<type_t, free_t, hold_by_t, allocated_t>::const_hold_t HResource<type_t, free_t, hold_by_t, allocated_t>::get( void ) const
	{
	return ( hold_by_t<value_type>::raw( _resource ) );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
typename HResource<type_t, free_t, hold_by_t, allocated_t>::hold_t HResource<type_t, free_t, hold_by_t, allocated_t>::get( void )
	{
	return ( hold_by_t<value_type>::raw( _resource ) );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
void HResource<type_t, free_t, hold_by_t, allocated_t>::reset( void )
	{
	if ( allocated_t::is_allocated( _resource ) )
		{
		_free( _resource );
		allocated_t::reset( _resource );
		}
	return;
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
typename HResource<type_t, free_t, hold_by_t, allocated_t>::hold_t HResource<type_t, free_t, hold_by_t, allocated_t>::release( void )
	{
	typename HResource<type_t, free_t, hold_by_t, allocated_t>::hold_t val( _resource );
	allocated_t::reset( _resource );
	return ( val );
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
void HResource<type_t, free_t, hold_by_t, allocated_t>::swap( HResource& other )
	{
	if ( &other != this )
		{
		using yaal::swap;
		swap( _resource, other._resource );
		swap( _free, other._free );
		}
	return;
	}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
template<typename alien_t>
void HResource<type_t, free_t, hold_by_t, allocated_t>::pass( alien_t& src_ )
	{
	HResource& src = reinterpret_cast<HResource&>( src_ );
	using yaal::swap;
	swap( _resource, src._resource );
	swap( _free, src._free );
	src_.reset();
	return;
	}

}

template<typename type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
inline void swap( yaal::hcore::HResource<type_t, free_t, hold_by_t, allocated_t>& a,
		yaal::hcore::HResource<type_t, free_t, hold_by_t, allocated_t>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HRESOURCE_HXX_INCLUDED */

