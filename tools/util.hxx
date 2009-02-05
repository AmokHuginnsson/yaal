/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	util.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/util.hxx
 * \brief Simple single function utilities.
 */

#ifndef YAAL_TOOLS_UTIL_HXX_INCLUDED
#define YAAL_TOOLS_UTIL_HXX_INCLUDED

#include "hcore/base.hxx"
#include "hcore/hstring.hxx"
#include "hcore/ooption.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Small but useful utils.
 */
namespace util
	{

template<typename iter_t, typename pred_t>
iter_t find_local( iter_t it, iter_t end, pred_t pred )
	{
	for ( ; it != end; ++ it )
		pred( it );
	return ( pred() );
	}

yaal::hcore::HString kwota_slownie( double );
void usun_ogonki( char* );
double long atof_ex( yaal::hcore::HString const&, bool = false );
bool verify_IBAN( yaal::hcore::HString const& );
char const* get_last_error( void );

void show_help( yaal::hcore::OOption*, int, char const* const, char const* const, char const* const = NULL );
void dump_configuration( yaal::hcore::OOption*, int, char const* const, char const* const, char const* const = NULL );
typedef void ( * OPTION_PROCESSOR_t )( yaal::hcore::OOption*, int, char const* const, char const* const, char const* const );
/*! \brief Helper structure for displaing program help and current configuration.
 */
struct OOptionInfo
	{
	yaal::hcore::OOption* _opt;
	int _size;
	OPTION_PROCESSOR_t PROC;
	OOptionInfo( yaal::hcore::OOption* opt, int const& size, OPTION_PROCESSOR_t const& a_PROC )
		: _opt( opt ), _size( size ), PROC( a_PROC ) {}
private:
	OOptionInfo( OOptionInfo const& );
	OOptionInfo& operator = ( OOptionInfo const& );
	};

void failure( int, char const* const, ... ) __attribute__(( __noreturn__ ));

/*! \brief Various convinient sleep functions.
 */
namespace sleep
{

int milisecond( int, bool = false );
int second( int, bool = false );

}

/*! \brief The Levenshtein and Levenshtein-Damerau string distance functions.
 */
namespace distance
{

/*! \brief Calculate distance between two strings.
 *
 * \param first - first string of a pair of strings to calculate distance.
 * \param second - second string of given pair.
 * \param damerau - use Levenshtein-Damerau definition of string distance if set to true, use Levenshtein definiton otherwise.
 * \return calculated Levenshtein(-Damerau) distance between strings.
 */
int levenshtein_damerau( yaal::hcore::HString const& first, yaal::hcore::HString const& second, bool damerau = true );

/*! \brief Predicate for find_local algorithm.
 *
 * HAlike looks for string most similiar to a given string.
 *
 * \tparam iter_t - type of an iterator that will be used to traverse collection.
 * \tparam item_t - type of item to look for.
 */
template<typename iter_t, typename item_t>
class HAlike 
	{
	item_t const& _item;
	iter_t _iter;
	int long _best;
	bool _damerau;
public:
	HAlike( iter_t it, item_t const& item, bool damerau = true ) : _item( item ), _iter( it ), _best( LONG_MAX ), _damerau( damerau ) {}
	void operator()( iter_t it )
		{
		int long dist = levenshtein_damerau( _item, *it, _damerau );
		if ( dist < _best )
			{
			_best = dist;
			_iter = it;
			}
		}
	iter_t operator()( void )
		{ return ( _iter ); }
	};

template<typename iter_t, typename item_t>
HAlike<iter_t, item_t> alike( iter_t iter, item_t const& item, bool damerau = true )
	{
	return ( HAlike<iter_t, item_t>( iter, item, damerau ) );
	}

}

	}

}

}

#endif /* not YAAL_TOOLS_UTIL_HXX_INCLUDED */
