/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/htwowaymap.hxx - this file is integral part of yaal project.

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
/*! \file tools/htwowaymap.hxx
 * \brief Declaration and implmentation of HTwoWayMap<> class template.
 */

#ifndef YAAL_TOOLS_HTWOWAYMAP_HXX_INCLUDED
#define YAAL_TOOLS_HTWOWAYMAP_HXX_INCLUDED 1

namespace yaal
{

namespace tools
{

/*! \brief HSBBSTree util, a helper for HSet<> instatiations.
 */
template<typename type_t>
struct twowaymap_helper
{

template<typename key_t>
inline static bool less( key_t const& left, key_t const& right )
	{	return ( left < right );	}

};

/*! \brief Bijective relation of elemnets.
 *
 * HTwoWayMap<> is a template representing pair of sets with ono-to-one correspondence
 * between their elements.
 *
 * \tparam left_type_t - type of elements in "left" set in map.
 * \tparam right_type_t - type of elements in "right" set in map.
 * \tparam left_helper_t - HSBBSTree plugable code.
 * \tparam right_helper_t - HSBBSTree plugable code.
 */
template<typename left_type_t, typename right_type_t,
	typename left_helper_t = twowaymap_helper<left_type_t const>,
	typename right_helper_t = twowaymap_helper<right_type_t const> >
class HTwoWayMap
	{
	};

}

}

#endif /* #ifndef YAAL_TOOLS_HTWOWAYMAP_HXX_INCLUDED */

