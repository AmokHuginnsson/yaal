/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hsearchablecontrol.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hsearchablecontrol.hxx"
#include "hwindow.hxx"
#include "hconsole.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace hconsole
{

HSearchableControl::HSearchableControl( bool a_bSearchable )
									: HControl( NULL, 0, 0, 0, 0, NULL ),
	f_bSearchable( a_bSearchable ), f_bSearchActived( false ),
	f_bFiltered( false ), f_bBackwards( false ), f_oPattern()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HSearchableControl::~HSearchableControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HSearchableControl::search( HString const& a_oPattern, bool a_bBackwards )
	{
	M_PROLOG
	HPattern::pluggable_flags_t pf;
	pf.push_back( make_pair( 'f', &f_bFiltered ) );
	f_bSearchActived = ! f_oPattern.parse( a_oPattern, &pf );
	if ( ! f_bSearchActived )
		f_poParent->status_bar()->message( f_oPattern.error().raw() );
	else
		{
		f_bBackwards = a_bBackwards;
		if ( f_bBackwards )
			go_to_match_previous();
		else
			go_to_match();
		}
	schedule_refresh();
	return;
	M_EPILOG
	}

void HSearchableControl::highlight( int a_iRow, int a_iColumn,
		int a_iCurrent, bool a_bCurrent )
	{
	M_PROLOG
	int long l_iCtr = 0;
	HConsole& cons = HCons::get_instance();
	for ( HPattern::HMatchIterator it = f_oPattern.find( f_oVarTmpBuffer.raw() ),
			end = f_oPattern.end(); it != end; ++ it )
		{
		if ( ( f_bFocused && ( ( a_iCurrent != l_iCtr ) || ! a_bCurrent ) )
				|| ( ! f_bFocused && ( a_iCurrent == l_iCtr ) && a_bCurrent ) )
			cons.set_attr( n_iAttributeSearchHighlight >> 8 );
		else
			cons.set_attr( n_iAttributeSearchHighlight );
		cons.c_mvprintf( a_iRow,
				static_cast<int>( a_iColumn + ( it->raw() - f_oVarTmpBuffer.raw() ) ),
				"%.*s", it->size(), it->raw() );
		l_iCtr ++;
		}
	return;
	M_EPILOG
	}

}

}

