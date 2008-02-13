/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hresource.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HRESOURCE_H
#define __YAAL_HCORE_HRESOURCE_H

namespace yaal
{

namespace hcore
{

struct OResourceHelper
	{
	static bool non_null( void* value )
		{ return ( value != NULL ); }
	static bool greater( int long value, int long than )
		{ return ( value > than ); }
	static bool greater_or_eq( int long value, int long than )
		{ return ( value >= than ); }
	};

template<typename resource_t, typename free_t, typename allocated_t = typeof( &OResourceHelper::non_null )>
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
			resource_t resource = f_tResource;
			allocated_t allocated = ALLOCATED;
			free_t free_ = FREE;
			f_tResource = other.f_tResource;
			ALLOCATED = other.ALLOCATED;
			FREE = other.FREE;
			other.f_tResource = resource;
			other.ALLOCATED = allocated;
			other.FREE = free_;
			}
		}
private:
	HResource( HResource const& );
	HResource& operator = ( HResource const& );
	};

}

}

#endif /* not __YAAL_HCORE_HRESOURCE_H */

