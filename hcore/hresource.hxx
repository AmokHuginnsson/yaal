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

namespace yaal
{

namespace hcore
{

/*! \brief Resource lifetime tracking helper.
 */
struct OResourceHelper
	{
	template<typename T>
	struct by_pointer
		{
		typedef T* type_t;
		typedef T* pass_t;
		static T* raw( T* val )
			{ return ( val ); }
		static T const* raw( T const* val )
			{ return ( val ); }
		};
	template<typename T>
	struct by_value
		{
		typedef T type_t;
		typedef T const& pass_t;
		static T raw( T& val )
			{ return ( val ); }
		static T raw( T const& val )
			{ return ( val ); }
		};
	struct always_release
		{
		static bool is_allocated( void* )
			{ return ( true ); }
		};
	struct non_null
		{
		static bool is_allocated( void* value )
			{ return ( value != NULL ); }
		};
	template<int than>
	struct greater
		{
		static bool is_allocated( int long value )
			{ return ( value > than ); }
		};
	template<int than>
	struct greater_or_eq
		{
		static bool is_allocated( int long value )
			{ return ( value >= than ); }
		};
	};

/*! \brief Raw resource life time tracker.
 */
template<typename resource_type_t, typename free_t, template<typename>class hold_by_t = OResourceHelper::by_pointer, typename allocated_t = OResourceHelper::non_null>
class HResource
	{
	typedef typename hold_by_t<resource_type_t>::type_t type_t;
	typedef typename hold_by_t<resource_type_t>::pass_t pass_t;
	type_t f_tResource;
	free_t FREE;
public:
	typedef resource_type_t resource_t;
	HResource( pass_t resource_, free_t free_ )
		: f_tResource( resource_ ), FREE( free_ ) {}
	~HResource( void )
		{
		if ( allocated_t::is_allocated( f_tResource ) )
			FREE( f_tResource );
		}
	resource_t operator*( void ) const
		{ return ( f_tResource ); }
	type_t get( void ) const
		{ return ( hold_by_t<resource_t>::raw( f_tResource ) ); }
	void swap( HResource& other )
		{
		if ( &other != this )
			{
			using yaal::swap;
			swap( f_tResource, other.f_tResource );
			swap( FREE, other.FREE );
			}
		}
private:
	HResource( HResource const& );
	HResource& operator = ( HResource const& );
	};

}

template<typename resource_t, typename free_t, template<typename>class hold_by_t, typename allocated_t>
inline void swap( yaal::hcore::HResource<resource_t, free_t, hold_by_t, allocated_t>& a,
		yaal::hcore::HResource<resource_t, free_t, hold_by_t, allocated_t>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HRESOURCE_HXX_INCLUDED */

