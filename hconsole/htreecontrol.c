/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	htreecontrol.c - this file is integral part of `stdhapi' project.

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

#include <string.h>

#include "../config.h"

#ifdef HAVE_NCURSES_H
#	include <ncurses.h>
#elif HAVE_NCURSES_NCURSES_H
#	include <ncurses/ncurses.h>
#else /* HAVE_NCURSES_NCURSES_H */
#	error "No ncurses header available."
#endif /* not HAVE_NCURSES_NCURSES_H */

#include "htreecontrol.h"

#include "../hcore/hexception.h"

HTreeControl::HNodeControl::HNodeControl ( HNodeControl * a_poNode )
						: HFlexibleTree::HFlexibleNode ( a_poNode )
	{
	M_PROLOG
	f_bUnfolded = false;
	return;
	M_EPILOG
	}

HTreeControl::HNodeControl::~HNodeControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HTreeControl::HNodeControl::expand ( void )
	{
	M_PROLOG
	f_bUnfolded = true;
	if ( f_poTrunk ) ( ( HTreeControl::HNodeControl * ) f_poTrunk )->expand ( );
	return;
	M_EPILOG
	}

void HTreeControl::HNodeControl::collapse ( void )
	{
	M_PROLOG
	int l_iFlag = ( int ) D_TREAT_AS_OPENED;
	f_bUnfolded = false;
  if  ( ! f_oBranch.quantity ( ) )return;
	f_oBranch.go ( 0 );
	while ( l_iFlag == ( int ) D_TREAT_AS_OPENED )
		( ( HTreeControl::HNodeControl * ) f_oBranch.to_tail ( 1, & l_iFlag ) )->collapse ( );
	return;
	M_EPILOG
	}

HTreeControl::HTreeControl ( HWindow * a_poParent, int a_iRow, int a_iColumn,
														 int a_iHeight, int a_iWidth,
														 const char * a_pcLabel )
						: HControl ( a_poParent, a_iRow, a_iColumn, a_iHeight, a_iWidth,
												 a_pcLabel )
	{
	M_PROLOG
	refresh ( );
	return;
	M_EPILOG
	}

HTreeControl::~HTreeControl ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HTreeControl::refresh ( void )
	{
	M_PROLOG
	int l_iCtr = 0;
	draw_label ( );
	f_oVarTmpBuffer.hs_realloc ( f_iWidthRaw + 1 );
	memset ( f_oVarTmpBuffer, '_', f_iWidthRaw );
	f_oVarTmpBuffer [ f_iWidthRaw ] = 0;
	console::set_attr ( f_bEnabled ? ( f_bFocused ? f_iFocusedAttribute
				: f_iEnabledAttribute ) : f_iAttribute );
	for ( l_iCtr = 0; l_iCtr < f_iHeightRaw; l_iCtr ++ )
		{
		move ( f_iRowRaw + l_iCtr, f_iColumnRaw );
		cprintf ( f_oVarTmpBuffer );
		}
	if ( f_poRoot )
		draw_node ( ( HNodeControl * ) f_poRoot, f_iRowRaw );
	curs_set ( 0 );
	return;
	M_EPILOG
	}

int HTreeControl::draw_node ( HNodeControl * a_poNode, int a_iRow )
	{
	M_PROLOG
	int l_iCtr = 0;
	int l_iRow = a_iRow;
	HInfo l_oInfo;
	l_iCtr = a_poNode->f_oBranch.quantity ( );
	if ( a_poNode->f_tLeaf.quantity ( ) )
		{ 
		l_iRow ++;
		l_oInfo = a_poNode->f_tLeaf [ 0 ];
		move ( l_iRow, f_iColumnRaw + a_poNode->f_iLevel * 2 - 1 );
		console::set_attr ( f_bEnabled ? ( f_bFocused ? f_iFocusedAttribute
					: f_iEnabledAttribute ) : f_iAttribute );
		if ( ! a_poNode->f_bUnfolded && l_iCtr )
			cprintf ( "+" );
		else if ( l_iCtr )cprintf ( "-" );
		if ( a_poNode == f_poSelected )
			console::set_attr ( f_bEnabled ? ( f_bFocused ? ~f_iFocusedAttribute
						: ~ f_iEnabledAttribute ) : ~ f_iAttribute );
		else
			console::set_attr ( f_bEnabled ? ( f_bFocused ? f_iFocusedAttribute
						: f_iEnabledAttribute ) : f_iAttribute );
		move ( l_iRow, f_iColumnRaw + a_poNode->f_iLevel * 2 );
		cprintf ( ( HString & ) l_oInfo );
		}
	if ( l_iCtr && ( a_poNode->f_bUnfolded || ! a_poNode->f_iLevel ) )
		{
		a_poNode->f_oBranch.go ( 0 );
		while ( l_iCtr -- )
			l_iRow = draw_node ( ( HNodeControl * ) a_poNode->f_oBranch.to_tail ( ),
					l_iRow );
		}
	return ( l_iRow );
	M_EPILOG
	}

int HTreeControl::process_input ( int a_iCode )
	{
	M_PROLOG
	int l_iErrorCode = 0;
	HNodeControl * l_poNode = NULL;
	a_iCode = HControl::process_input ( a_iCode );
	l_poNode = ( HNodeControl * ) f_poSelected;
	switch ( a_iCode )
		{
		case ( KEY_HOME ):
			{
			l_poNode = ( HNodeControl * ) f_poRoot;
			if ( l_poNode->f_oBranch.quantity ( ) )
				f_poSelected = l_poNode->f_oBranch [ 0 ];
			break;
			}
		case ( KEY_END ):
			{
			l_poNode = ( HNodeControl * ) f_poRoot;
			if ( l_poNode->f_oBranch.quantity ( ) )
				{
				l_poNode->f_oBranch.go ( 0 );
				f_poSelected = l_poNode->f_oBranch.to_head ( );
				}
			break;
			}
		case ( KEY_PPAGE ):
			{
			break;
			}
		case ( KEY_NPAGE ):
			{
			break;
			}
		case ( KEY_UP ):
			{
			f_poSelected = l_poNode->previous ( );
			if ( f_poSelected == l_poNode )
				{
				if ( l_poNode->f_iLevel > 1 ) 
					f_poSelected = l_poNode->f_poTrunk;
				}
			else
				{
				l_poNode = ( HNodeControl * ) f_poSelected;
				while ( l_poNode->f_bUnfolded )
					{
					if ( l_poNode->f_oBranch.quantity ( ) )
						{
						f_poSelected = l_poNode->f_oBranch [ 0 ];
						l_poNode = ( HNodeControl * ) f_poSelected;
						f_poSelected = l_poNode->previous ( D_TREAT_AS_CLOSED );
						l_poNode = ( HNodeControl * ) f_poSelected;
						}
					}
				}
			break;
			}
		case ( KEY_DOWN ):
			{
			if ( l_poNode->f_bUnfolded )
				{
				if ( l_poNode->f_oBranch.quantity ( ) )
					f_poSelected = l_poNode->f_oBranch [ 0 ];
				}
			else
				{
				f_poSelected = l_poNode->next ( );
				while ( l_poNode->f_iLevel > 1 )
					if ( f_poSelected == l_poNode ) 
						{
						f_poSelected = ( ( HNodeControl * ) l_poNode->f_poTrunk )->next ( );
						l_poNode = ( HNodeControl * )f_poSelected;
						}
					else break;
				}
			break;
			}
		case ( KEY_LEFT ):
			{
			if ( l_poNode->f_bUnfolded && l_poNode->f_iLevel )
				l_poNode->f_bUnfolded = false;
			else if ( l_poNode->f_iLevel > 1 )
				f_poSelected = l_poNode->f_poTrunk;
			break;
			}
		case ( KEY_RIGHT ):
			{
			if ( l_poNode->f_bUnfolded )
				{
				if ( l_poNode->f_oBranch.quantity ( ) )
					f_poSelected = l_poNode->f_oBranch [ 0 ];
				else
					{
					}
				}
			else if ( l_poNode->f_oBranch.quantity ( ) )
				l_poNode->f_bUnfolded = true;
			else
				{
				f_poSelected = l_poNode->next ( );
				while ( l_poNode->f_iLevel > 1 )
					if ( f_poSelected == l_poNode ) 
						{
						f_poSelected = ( ( HNodeControl * ) l_poNode->f_poTrunk )->next ( );
						l_poNode = ( HNodeControl * )f_poSelected;
						}
					else break;
				}
			break;
			}
		case ( ' ' ):
		case ( '\r' ):
			{
			if ( l_poNode->f_oBranch.quantity ( ) )
				l_poNode->f_bUnfolded = ! l_poNode->f_bUnfolded;
			else l_iErrorCode = a_iCode;
			break; /* I have to think about it more. */
			}
		case ( '\t' ):
			{
			f_bFocused = false;	/* very  */
			refresh ( );				/* magic */
			}
		default :
			{
			l_iErrorCode = a_iCode;
			break;
			}
		}
	a_iCode = l_iErrorCode;
	if ( ! l_iErrorCode )
		{
		refresh ( );
		console::c_printf ( console::n_iHeight - 1, -1, 7, "" );
		}
	return ( a_iCode );
	M_EPILOG
	}

int HTreeControl::set_focus ( char a_cShorcut )
	{
	M_PROLOG
	curs_set ( 0 );
	return ( HControl::set_focus ( a_cShorcut ) );
	M_EPILOG
	}

