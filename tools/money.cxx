/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  money.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>
#include <cctype>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "money.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace money {

namespace {
	static int const MIN_IBAN_LENGTH = 20;
	static int const STEP_LENGTH = 4;
}

namespace en {

char _unit_[][ 14 ] = {
	"thousand ",
	"million ",
	"billion ",
	"trillion ",
	"quadrillion ",
	"quintillion ",
	"sextillion "
};

char _numbers_[][ 12 ] = {
	"zero ",
	"one ",
	"two ",
	"three ",
	"four ",
	"five ",
	"six ",
	"seven ",
	"eight ",
	"nine ",
	"ten ",
	"eleven ",
	"twelve ",
	"thirteen ",
	"fourteen ",
	"fifteen ",
	"sixteen ",
	"seventeen ",
	"eighteen ",
	"nineteen "
};

char const _tenths_[][ 10 ] = {
	"zero-ten",
	"ten ",
	"twenty ",
	"thirty ",
	"forty ",
	"fifty ",
	"sixty ",
	"seventy ",
	"eighty ",
	"ninety "
};

char const _houndred_[] = "hundred ";

namespace currency {

char const _dollar_[][14] = {
	"cent",
	"dollar"
};

char const _dollarEnd_[][2][8] = {
	{ "", "s" },
	{ " and ", "s and " }
};

char const _pound_[][14] = {
	"pen",
	"pound"
};

char const _poundEnd_[][2][8] = {
	{ "ny", "ce" },
	{ " and ", "s and " }
};

char const _euro_[][14] = {
	"eurocent",
	"euro"
};

char const _euroEnd_[][2][8] = {
	{ "", "s" },
	{ " and ", "s and " }
};

char const _pln_[][14] = {
	"grosz",
	"PLN"
};

char const _plnEnd_[][2][8] = {
	{ "", "y" },
	{ " and ", "s and " }
};

}

typedef char const (*currency_t)[14];
currency_t _currency_[] = {
	currency::_dollar_,
	currency::_pound_,
	currency::_euro_,
	currency::_pln_
};

typedef char const (*currency_end_t)[2][8];
currency_end_t const _currencyEnd_[] = {
	currency::_dollarEnd_,
	currency::_poundEnd_,
	currency::_euroEnd_,
	currency::_plnEnd_
};

}

namespace pl {

char const _numbers_[][ 16 ] = {
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

char const _tenths_[][ 24 ] = {
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

char const _houndreds_[][ 16 ] = {
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

namespace currency {

char const _dollar_[][ 12 ] = {
	"cent",
	"dollar"
};

char const _dollarEnd_[][3][6] = {
	{ "", "y", "ów" },
	{ " ", "y ", "ów " }
};

char const _pound_[][ 12 ] = {
	"pens",
	"funt"
};

char const _poundEnd_[][3][6] = {
	{ "", "y", "ów" },
	{ " ", "y ", "ów " }
};

char const _euro_[][ 12 ] = {
	"eurocent",
	"euro"
};

char const _euroEnd_[][3][6] = {
	{ "", "y", "ów" },
	{ " ", " ", " " }
};

char const _pln_[][ 12 ] = {
	"grosz",
	"z³ot"
};

char const _plnEnd_[][3][6] = {
	{ "", "e", "y" },
	{ "y ", "e ", "ych " }
};

}

typedef char const (*currency_t)[12];
currency_t const _currency_[] = {
	currency::_dollar_,
	currency::_pound_,
	currency::_euro_,
	currency::_pln_
};

typedef char const (*currency_end_t)[3][6];
currency_end_t const _currencyEnd_[] = {
	currency::_dollarEnd_,
	currency::_poundEnd_,
	currency::_euroEnd_,
	currency::_plnEnd_
};

char _unit_[][ 12 ] = {
	"tysi",
	"milion",
	"miliard",
	"bilion",
	"biliard",
	"trylion",
	"tryliard"
};

char _unitEnd_[][ 3 ][ 6 ] = {
	{ "±c ", "±ce ", "êcy " },
	{ " ", "y ", "ów " },
	{ " ", "y ", "ów " },
	{ " ", "y ", "ów " },
	{ " ", "y ", "ów " },
	{ " ", "y ", "ów " },
	{ " ", "y ", "ów " }
};

}

HString money_string( HNumber const& amount_ ) {
	M_PROLOG
	HNumber n( amount_ );
	n.round( 2 );
	HString string( n.to_string() );
	int long pos( string.reverse_find( '.' ) );
	if ( pos == HString::npos )
		string += ".00";
	else if ( pos == 1 )
		string += "0";
	return ( string );
	M_EPILOG
}

HString in_words_en( HNumber const& kwota_, CURRENCY currency_ ) {
	M_PROLOG
	int form( 0 );
	HString inWords;
	HString string( money_string( kwota_ ) );
	HString tmp;
	int long const length( string.get_length() );
	for ( int i( 0 ); i < length; ++ i ) {
		if ( ( i % 3 ) == 0 ) {
			int sub( ( length - i ) > 1 ? 2 : 1 );
			form = lexical_cast<int>( string.mid( ( length - i ) - sub, sub ) );
			if ( ( i > 5 ) && ( ( length - i ) > 2 ) &&
					! ::strncmp( string.raw() + ( length - i ) - 3, "000", 3 ) ) {
				continue;
			}
		}
		if ( i == 2 ) {
			continue;
		}
		char digit( static_cast<char>( string[ ( length - i ) - 1 ] - '0' ) );
		switch ( i % 3 ) {
			case ( 0 ) : {
				int unitIdx( i / 3 );
				typedef char const (*unit_t)[14];
				unit_t const unit( unitIdx >= 2 ? en::_unit_ : en::_currency_[static_cast<int>( currency_ )] );
				if ( unitIdx < 2 ) {
					inWords = en::_currencyEnd_[static_cast<int>(currency_)][ unitIdx ][( form == 1 ) && ( ( unitIdx == 0 ) || ( length == 4 ) ) ? 0 : 1] + inWords;
				}
				if ( unitIdx >= 2 ) {
					unitIdx -= 2;
				}
				inWords = unit[ unitIdx ] + inWords;
				if ( ( form < 20 ) &&  ( form > 9 ) ) {
					inWords = en::_numbers_[ form ] + inWords;
				} else if ( digit ) {
					inWords = en::_numbers_[ static_cast<int>( digit ) ] + inWords;
				} else if ( ! form && ( ( i < 3 ) || ( kwota_ < 1 ) ) ) {
					inWords = en::_numbers_[ 0 ] + inWords;
				}
			}
			break;
			case ( 1 ) : {
				if ( form > 19 ) {
					tmp.assign( en::_tenths_[ static_cast<int>( digit ) ] );
					if ( form % 10 ) {
						tmp.set_at( tmp.get_length() - 1, '-' );
					}
					inWords = tmp + inWords;
				}
			} break;
			case ( 2 ) : {
				if ( digit ) {
					tmp.assign( en::_numbers_[ static_cast<int>( digit ) ] ).append( en::_houndred_ );
					inWords = tmp + inWords;
				}
			} break;
			default:
			break;
		}
	}
	return ( inWords );
	M_EPILOG
}

HString in_words_pl( HNumber const& kwota_, CURRENCY currency_ ) {
	M_PROLOG
	int form( 0 );
	HString inWords;
	HString instance;
	HString string( money_string( kwota_ ) );
	int long length( string.get_length() );
	for ( int i( 0 ); i < length; i ++ ) {
		if ( ( i % 3 ) == 0 ) {
			int sub( ( length - i ) > 1 ? 2 : 1 );
			form = lexical_cast<int>( string.mid( ( length - i ) - sub, sub ) );
			if ( ( i > 5 ) && ( ( length - i ) > 2 ) &&
					! ::strncmp( string.raw() + ( length - i ) - 3, "000", 3 ) ) {
				continue;
			}
		}
		if ( i == 2 ) {
			continue;
		}
		char digit( static_cast<char>( string[ ( length - i ) - 1 ] - '0' ) );
		switch ( i % 3 ) {
			case ( 0 ) : {
				int unitIdx( i / 3 );
				typedef char const (*unit_t)[12];
				unit_t const unit( unitIdx >= 2 ? pl::_unit_ : pl::_currency_[static_cast<int>( currency_ )] );
				typedef char const (*end_t)[3][6];
				end_t const end( unitIdx >= 2 ? pl::_unitEnd_ : pl::_currencyEnd_[static_cast<int>( currency_ )] );
				if ( unitIdx >= 2 ) {
					unitIdx -= 2;
				}
				instance = unit[ unitIdx ];
				if ( form == 1 ) {
					instance += end[ unitIdx ] [ 0 ];
				} else if ( ( ( ( form % 10 ) > 1 ) && ( ( form % 10 ) < 5 ) )
						&& ( ( form < 10 ) || ( form > 20 ) ) ) {
					instance += end[ unitIdx ] [ 1 ];
				} else {
					instance += end[ unitIdx ] [ 2 ];
				}
				inWords = instance + inWords;
				if ( ( form < 20 ) &&  ( form > 9 ) ) {
					inWords = pl::_numbers_[ form ] + inWords;
				} else if ( digit ) {
					inWords = pl::_numbers_[ static_cast<int>( digit ) ] + inWords;
				} else if ( ! form && ( ( i < 3 ) || ( kwota_ < 1 ) ) ) {
					inWords = pl::_numbers_[ 0 ] + inWords;
				}
			}
			break;
			case ( 1 ) : {
				if ( form > 19 ) {
					inWords = pl::_tenths_[ static_cast<int>( digit ) ] + inWords;
				}
			} break;
			case ( 2 ) : {
				if ( digit ) {
					inWords = pl::_houndreds_[ static_cast<int>( digit ) ] + inWords;
				}
			} break;
			default:
			break;
		}
	}
	return ( inWords );
	M_EPILOG
}

namespace {

static HString _lastErrorMessage_;

int modulo_ASCII( HString const& aSCIINumber_, int modulo_ ) {
	M_PROLOG
	int ctr = 0, number = 0, step = 0;
	int long tmpLength = 0;
	int long length = aSCIINumber_.get_length();
	HString tmpString, tmpNumber = aSCIINumber_;
	if ( length < 0 )
		M_THROW( "bad ASCII number length", length );
	if ( ! modulo_ )
		M_THROW( "zero denominatior", modulo_ );
	while ( tmpNumber.get_length() > STEP_LENGTH ) {
		tmpString = tmpNumber.mid( step * STEP_LENGTH, STEP_LENGTH );
		tmpLength = tmpString.get_length();
		number = lexical_cast<int>( tmpString );
		number %= modulo_;
		tmpString.format ( "%d", number );
		tmpNumber.shift_left( tmpLength - tmpString.get_length() );
		tmpLength = tmpString.get_length();
		for ( ctr = 0; ctr < tmpLength; ctr ++ ) {
			tmpNumber.set_at( ctr, tmpString[ ctr ] );
		}
/*		M_LOG ( tmpNumber ); */
	}
	return ( lexical_cast<int>( tmpNumber ) % modulo_ );
	M_EPILOG
}

}

char const* get_last_error( void ) {
	if ( ! _lastErrorMessage_.is_empty() )
		return ( _lastErrorMessage_.raw() );
	return ( "" );
}

bool verify_IBAN( HString const& IBAN_ ) {
	M_PROLOG
	int ctr = 0;
	int long length = IBAN_.get_length();
	char pattern[ 2 ] = "\0";
	HString IBAN, tmpString;
	bool valid( false );
	do {
		if ( length < MIN_IBAN_LENGTH ) {
			_lastErrorMessage_.format( "IBAN: Number too short (%d).", static_cast<int>( length ) );
			break;
		}
		IBAN.reserve( length );
		for ( ctr = 0; ctr < length; ctr ++ )
			if ( isalnum( IBAN_[ ctr ] ) )
				IBAN += IBAN_[ ctr ];
		length = IBAN.get_length();
		if ( length < MIN_IBAN_LENGTH ) {
			_lastErrorMessage_.format( "IBAN: Number too short (%d).", static_cast<int>( length ) );
			break;
		}
		if ( ! ( isalpha( IBAN[ 0 ] ) && isalpha( IBAN[ 1 ] ) ) ) {
			_lastErrorMessage_ = "IBAN: No country code present.";
			break;
		}
		tmpString = IBAN.left( 4 );
		IBAN.shift_left ( 4 );
		IBAN += tmpString;
	/*	M_LOG ( IBAN ); */
		IBAN.lower();
		for ( ctr = 0; ctr < length; ctr ++ ) {
			if ( isalpha ( IBAN[ ctr ] ) ) {
				tmpString.format( "%02d", ( IBAN[ ctr ] - 'a' ) + 10 );
				pattern[ 0 ] = IBAN[ ctr ];
				IBAN.replace( pattern, tmpString );
				length = IBAN.get_length();
			}
		}
	/*	M_LOG ( IBAN ); */
		ctr = modulo_ASCII( IBAN, 97 );
		if ( ctr != 1 ) {
			_lastErrorMessage_.format( "IBAN: bad checksum: %d", ctr );
			break;
		}
		valid = true;
	} while ( false );
	return ( valid );
	M_EPILOG
}

}

}

}

