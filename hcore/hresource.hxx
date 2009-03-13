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
	static bool non_null( void* value )
		{ return ( value != NULL ); }
	static bool greater( int long value, int long than )
		{ return ( value > than ); }
	static bool greater_or_eq( int long value, int long than )
		{ return ( value >= than ); }
	};

/*! \brief Raw resource life time tracker.
 */
template<typename resource_t, typename free_t, typename allocated_t = __decltype( &OResourceHelper::non_null )>
class HResource
	{
	resource_t f_tResource;
	allocated_t ALLOCATED;
	free_t FREE;
public:
	HResource( resource_t resource, free_t free, allocated_t allocated = OResourceHelper::non_null )
		: f_tResource( resource ), ALLOCATED( allocated ), FREE( free ) {}
	~HResource( void )
		{
		if ( ALLOCATED( f_tResource ) )
			FREE( f_tResource );
		}
	resource_t get( void ) const
		{ return ( f_tResource ); }
	void swap( HResource& other )
		{
		if ( &other != this )
			{
			using yaal::swap;
			swap( f_tResource, other.f_tResource );
			swap( ALLOCATED, other.ALLOCATED );
			swap( FREE, other.FREE );
			}
		}
private:
	HResource( HResource const& );
	HResource& operator = ( HResource const& );
	};

}

template<typename resource_t, typename free_t, typename allocated_t>
inline void swap( yaal::hcore::HResource<resource_t, free_t, allocated_t>& a, yaal::hcore::HResource<resource_t, free_t, allocated_t>& b )
	{ a.swap( b ); }

}

#endif /* not YAAL_HCORE_HRESOURCE_HXX_INCLUDED */

