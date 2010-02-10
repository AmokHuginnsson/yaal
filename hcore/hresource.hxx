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

/*! \file Declaration of HResource<> class.
 *
 * Scoped reasource guardian with definable resource free freeing function.
 */

#ifndef YAAL_HCORE_HRESOURCE_HXX_INCLUDED
#define YAAL_HCORE_HRESOURCE_HXX_INCLUDED

#include "hcore/base.hxx"

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
		typedef T* hold_t;
		typedef T const* const_hold_t;
		typedef T* ref_t;
		typedef T const* const_ref_t;
		static T* raw( T* val )
			{ return ( val ); }
		static T const* raw( T const* val )
			{ return ( val ); }
		};
	template<typename T>
	struct by_value
		{
		typedef T hold_t;
		typedef T const& const_hold_t;
		typedef T& ref_t;
		typedef T const& const_ref_t;
		static T raw( T& val )
			{ return ( val ); }
		static T raw( T const& val )
			{ return ( val ); }
		};
	};
template<typename T>
struct HResourceReleaseBy
	{
	template<typename real_t>
	static void delete_obj( T p )
		{
		delete static_cast<real_t>( p );
		}
	template<typename real_t>
	static void delete_array( T p )
		{
		delete [] static_cast<real_t>( p );
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

template<typename resource_type_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
struct HResourceRef
	{
	typedef typename hold_by_t<resource_type_t>::hold_t hold_t;
	hold_t _hold;
	free_t _free;
	explicit HResourceRef( hold_t hold_, free_t free_ ) : _hold( hold_ ), _free( free_ ) {}
	};

/*! \brief Raw resource life time tracker.
 */
template<typename resource_type_t, typename free_t = void (*)( resource_type_t* ), template<typename>class hold_by_t = HResourceAllocatedBy::by_pointer, typename allocated_t = HResourceReleaseWhen::non_null>
class HResource
	{
	typedef typename hold_by_t<resource_type_t>::hold_t hold_t;
	typedef typename hold_by_t<resource_type_t>::const_hold_t const_hold_t;
	typedef typename hold_by_t<resource_type_t>::ref_t ref_t;
	typedef typename hold_by_t<resource_type_t>::const_ref_t const_ref_t;
	hold_t _resource;
	free_t _free;
public:
	typedef resource_type_t resource_t;
	HResource( void ) : _resource(), _free() {}
	template<typename real_t>
	explicit HResource( real_t resource_, free_t free_ = &HResourceReleaseBy<hold_t>::template delete_obj<real_t> )
		: _resource( resource_ ), _free( free_ ) {}
	~HResource( void )
		{
		reset();
		return;
		}
	HResource( HResource& src_ ) : _resource(), _free()
		{
		pass( src_ );
		return;
		}
	template<typename real_t>
	HResource( HResource<real_t, free_t, hold_by_t, allocated_t>& src_ ) : _resource(), _free()
		{
		pass( src_ );
		return;
		}
	HResource& operator = ( HResource& src_ )
		{
		if ( &src_ != this )
			pass( src_ );
		return ( *this );
		}
	template<typename real_t, typename real_free_t>
	HResource& operator = ( HResource<real_t, real_free_t, hold_by_t, allocated_t>& src_ )
		{
		if ( &reinterpret_cast<HResource&>( src_ ) != this )
			pass( src_ );
		return ( *this );
		}
	HResource( HResourceRef<resource_type_t, free_t, hold_by_t, allocated_t> src_ ) : _resource( src_._hold ), _free( src_._free ) {}
	HResource& operator = ( HResourceRef<resource_type_t, free_t, hold_by_t, allocated_t> src_ )
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
	void reset( void )
		{
		if ( allocated_t::is_allocated( _resource ) )
			{
			_free( _resource );
			allocated_t::reset( _resource );
			}
		return;
		}
	const_ref_t operator*( void ) const
		{ return ( _resource ); }
	ref_t operator*( void )
		{ return ( _resource ); }
	const_hold_t operator->( void ) const
		{ return ( hold_by_t<resource_t>::raw( _resource ) ); }
	hold_t operator->( void )
		{ return ( hold_by_t<resource_t>::raw( _resource ) ); }
	const_hold_t get( void ) const
		{ return ( hold_by_t<resource_t>::raw( _resource ) ); }
	hold_t get( void )
		{ return ( hold_by_t<resource_t>::raw( _resource ) ); }
	template<typename real_t>
	operator HResourceRef<real_t, free_t, hold_by_t, allocated_t>( void )
		{
		HResourceRef<real_t, free_t, hold_by_t, allocated_t> ref( _resource, _free );
		_resource = typename hold_by_t<resource_t>::hold_t();
		_free = free_t();
		return ( ref );
		}
	template<typename real_t>
	operator HResource<real_t, free_t, hold_by_t, allocated_t>( void )
		{
		HResource<real_t, free_t, hold_by_t, allocated_t> ref( _resource, _free );
		_resource = typename hold_by_t<resource_t>::hold_t();
		_free = free_t();
		return ( ref );
		}
	void swap( HResource& other )
		{
		if ( &other != this )
			{
			using yaal::swap;
			swap( _resource, other._resource );
			swap( _free, other._free );
			}
		}
private:
	template<typename alien_t>
	void pass( alien_t& src_ )
		{
		HResource& src = reinterpret_cast<HResource&>( src_ );
		using yaal::swap;
		swap( _resource, src._resource );
		swap( _free, src._free );
		src_.reset();
		return;
		}
	};

}

template<typename resource_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
inline void swap( yaal::hcore::HResource<resource_t, free_t, hold_by_t, allocated_t>& a,
		yaal::hcore::HResource<resource_t, free_t, hold_by_t, allocated_t>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HRESOURCE_HXX_INCLUDED */

