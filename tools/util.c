/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	util.c - this file is integral part of `stdhapi' project.

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

#include <stdlib.h>

#include "../hcore/hexception.h"
M_CVSID ( "$CVSHeader$" );
#include "util.h"

namespace util
{

char n_ppcJednNastki [ ] [ 16 ] =
	{
	"zero ",
	"jeden ",
	"dwa ",
	"trzy ",
	"cztery ",
	"piêæ ",
	"sze¶æ ",
	"siedem ",
	"osiem ",
	"dziewiêæ ",
	"dziesiêæ ",
	"jedena¶cie ",
	"dwana¶cie ",
	"trzyna¶cie ",
	"czterna¶cie ",
	"piêtna¶cie ",
	"szesna¶cie ",
	"siedemna¶cie ",
	"osiemna¶cie ",
	"dziewiêtna¶cie ",
	"dwadzie¶cia "
	};

char n_ppcDzies [ ] [ 24 ] =
	{
	"dziesiêæ-zero ",
	"dziesiêæ ",
	"dwadzie¶cia ",
	"trzydzie¶ci ",
	"czterdzie¶ci ",
	"piêædziesi±t ",
	"sze¶ædziesi±t ",
	"siedemdziesi±t ",
	"osiemdziesi±t ",
	"dziewiêædziesi±t ",
	"sto "
	};

char n_ppcSetki [ ] [ 16 ] =
	{
	"sto-zero ",
	"sto ",
	"dwie¶cie ",
	"trzysta ",
	"czterysta ",
	"piêæset ",
	"sze¶æset ",
	"siedemset ",
	"osiemset ",
	"dziewiêæset ",
	"tysi±c "
	};

char n_ppcTemat [ ] [ 12 ] =
	{
	"grosz",
	"z³ot",
	"tysi",
	"milion",
	"miliard",
	"bilion",
	"biliard",
	"trylion",
	"tryliard"
	};

char n_pppcKoncowka [ ] [ 3 ] [ 6 ] =
	{
		{
		"",
		"e",
		"y"
		},
		{
		"y ",
		"e ",
		"ych "
		},
		{
		"±c ",
		"±ce ",
		"êcy "
		},
		{
		" ",
		"y ",
		"ów "
		},
		{
		" ",
		"y ",
		"ów "
		},
		{
		" ",
		"y ",
		"ów "
		},
		{
		" ",
		"y ",
		"ów "
		},
		{
		" ",
		"y ",
		"ów "
		},
		{
		" ",
		"y ",
		"ów "
		}
	};

HString kwota_slownie ( double a_dKwota )
	{
	M_PROLOG
	int l_iCtr = 0, l_iLength = 0, l_iForma = 0, l_iSub = 0;
	char l_cCyfra = 0;
	HString l_oSlownie;
	HString l_oString;
	HString l_oPrzypadek;
	l_oString.format ( "%.2f", a_dKwota );
	l_iLength = l_oString.get_length ( );
	for ( l_iCtr = 0; l_iCtr < l_iLength; l_iCtr ++ )
		{
		if ( ( l_iCtr % 3 ) == 0 )
			{
			l_iSub = ( ( l_iLength - l_iCtr ) > 1 ? 2 : 1 );
			l_iForma = atoi ( l_oString.mid ( l_iLength - l_iCtr - l_iSub, l_iSub ) );
			}
		if ( l_iCtr == 2 )continue;
		l_cCyfra = l_oString [ l_iLength - l_iCtr - 1 ] - '0';
		switch ( l_iCtr % 3 )
			{
			case ( 0 ) :
				{
				l_oPrzypadek = n_ppcTemat [ l_iCtr / 3 ];
				if ( l_iForma == 1 )l_oPrzypadek += n_pppcKoncowka [ l_iCtr / 3 ] [ 0 ];
				else if ( ( ( ( l_iForma % 10 ) > 1 ) && ( ( l_iForma % 10 ) < 5 ) ) && ( ( l_iForma < 10 ) || ( l_iForma > 20 ) ) )l_oPrzypadek += n_pppcKoncowka [ l_iCtr / 3 ] [ 1 ];
				else l_oPrzypadek += n_pppcKoncowka [ l_iCtr / 3 ] [ 2 ];
				l_oSlownie = l_oPrzypadek + l_oSlownie;
				if ( ( l_iForma < 20 ) &&  ( l_iForma > 9 ) )
					l_oSlownie = n_ppcJednNastki [ l_iForma ] + l_oSlownie;
				else if ( l_cCyfra )
					l_oSlownie = n_ppcJednNastki [ ( int ) l_cCyfra ] + l_oSlownie;
				else if ( ! l_iForma && ( ( l_iCtr < 3 ) || ( a_dKwota < 1 ) ) )
					l_oSlownie = n_ppcJednNastki [ 0 ] + l_oSlownie;
				break;
				}
			case ( 1 ) :
				{
				if ( l_iForma > 19 )
					l_oSlownie = n_ppcDzies [ ( int ) l_cCyfra ] + l_oSlownie;
				break;
				}
			case ( 2 ) :
				{
				if ( l_cCyfra )
					l_oSlownie = n_ppcSetki [ ( int ) l_cCyfra ] + l_oSlownie;
				break;
				}
			}
		}
	return ( l_oSlownie );
	M_EPILOG
	}

};
