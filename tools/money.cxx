/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cstdio>
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

char const _hundred_[] = "hundred ";

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

char const _numbers_[][ 20 ] = {
	"zero ",
	"jeden ",
	"dwa ",
	"trzy ",
	"cztery ",
	"pięć ",
	"sześć ",
	"siedem ",
	"osiem ",
	"dziewięć ",
	"dziesięć ",
	"jedenaście ",
	"dwanaście ",
	"trzynaście ",
	"czternaście ",
	"piętnaście ",
	"szesnaście ",
	"siedemnaście ",
	"osiemnaście ",
	"dziewiętnaście ",
	"dwadzieścia "
};

char const _tenths_[][ 24 ] = {
	"dziesięć-zero ",
	"dziesięć ",
	"dwadzieścia ",
	"trzydzieści ",
	"czterdzieści ",
	"pięćdziesiąt ",
	"sześćdziesiąt ",
	"siedemdziesiąt ",
	"osiemdziesiąt ",
	"dziewięćdziesiąt ",
	"sto "
};

char const _hundreds_[][ 16 ] = {
	"sto-zero ",
	"sto ",
	"dwieście ",
	"trzysta ",
	"czterysta ",
	"pięćset ",
	"sześćset ",
	"siedemset ",
	"osiemset ",
	"dziewięćset ",
	"tysiąc "
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
	"złot"
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
	"m" "i" "l" "i" "o" "n", /* codespell */
	"miliard",
	"bilion",
	"biliard",
	"trylion",
	"tryliard"
};

char _unitEnd_[][ 3 ][ 6 ] = {
	{ "ąc ", "ące ", "ęcy " },
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
	int long pos( string.reverse_find( '.'_ycp ) );
	if ( pos == HString::npos )
		string += ".00";
	else if ( pos == 1 )
		string += "0";
	return string;
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
			if ( ( i > 5 ) && ( ( length - i ) > 2 ) && ( string.substr( ( length - i ) - 3, 3 ) == "000" ) ) {
				continue;
			}
		}
		if ( i == 2 ) {
			continue;
		}
		char digit( static_cast<char>( string[ ( length - i ) - 1 ].get() - '0' ) );
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
						tmp.set_at( tmp.get_length() - 1, '-'_ycp );
					}
					inWords = tmp + inWords;
				}
			} break;
			case ( 2 ) : {
				if ( digit ) {
					tmp.assign( en::_numbers_[ static_cast<int>( digit ) ] ).append( en::_hundred_ );
					inWords = tmp + inWords;
				}
			} break;
			default:
			break;
		}
	}
	return inWords;
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
			int sub( min( i == 0 ? 2 : 3, static_cast<int>( length ) - i ) );
			form = lexical_cast<int>( string.mid( ( length - i ) - sub, sub ) );
			if ( ( i > 5 ) && ( ( length - i ) > 2 ) && ( string.substr( ( length - i ) - 3, 3 ) == "000" ) ) {
				continue;
			}
		}
		if ( i == 2 ) {
			continue;
		}
		char digit( static_cast<char>( string[ ( length - i ) - 1 ].get() - '0' ) );
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
				} else if (
					( ( ( form % 10 ) > 1 ) && ( ( form % 10 ) < 5 ) )
					&& ( ( ( form % 100 ) < 10 ) || ( ( form % 100 ) > 20 ) )
				) {
					instance += end[ unitIdx ] [ 1 ];
				} else {
					instance += end[ unitIdx ] [ 2 ];
				}
				inWords = instance + inWords;
				if ( ( ( form % 100 ) < 20 ) &&  ( ( form % 100 ) > 9 ) ) {
					inWords = pl::_numbers_[ form ] + inWords;
				} else if ( digit ) {
					inWords = pl::_numbers_[ static_cast<int>( digit ) ] + inWords;
				} else if ( ! ( form % 100 ) && ( ( i < 3 ) || ( kwota_ < 1 ) ) ) {
					inWords = pl::_numbers_[ 0 ] + inWords;
				}
			}
			break;
			case ( 1 ) : {
				if ( ( form % 100 ) > 19 ) {
					inWords = pl::_tenths_[ static_cast<int>( digit ) ] + inWords;
				}
			} break;
			case ( 2 ) : {
				if ( digit ) {
					inWords = pl::_hundreds_[ static_cast<int>( digit ) ] + inWords;
				}
			} break;
			default:
			break;
		}
	}
	return inWords;
	M_EPILOG
}

namespace {

static HString _lastErrorMessage_;

int modulo_ASCII( HString const& aSCIINumber_, int modulo_ ) {
	M_PROLOG
	int number( 0 );
	int step( 0 );
	int long tmpLength( 0 );
	int long length( aSCIINumber_.get_length() );
	HString tmpString;
	HString tmpNumber( aSCIINumber_ );
	if ( length < 0 ) {
		M_THROW( "bad ASCII number length", length );
	}
	if ( ! modulo_ ) {
		M_THROW( "zero denominator", modulo_ );
	}
	while ( tmpNumber.get_length() > STEP_LENGTH ) {
		tmpString = tmpNumber.mid( step * STEP_LENGTH, STEP_LENGTH );
		tmpLength = tmpString.get_length();
		number = lexical_cast<int>( tmpString );
		number %= modulo_;
		tmpString = number;
		tmpNumber.shift_left( tmpLength - tmpString.get_length() );
		tmpLength = tmpString.get_length();
		for ( int ctr( 0 ); ctr < tmpLength; ctr ++ ) {
			tmpNumber.set_at( ctr, tmpString[ ctr ] );
		}
/*		M_LOG ( tmpNumber ); */
	}
	return ( lexical_cast<int>( tmpNumber ) % modulo_ );
	M_EPILOG
}

}

yaal::hcore::HString const& get_last_error( void ) {
	return ( _lastErrorMessage_ );
}

bool verify_IBAN( HString const& IBAN_ ) {
	M_PROLOG
	int long length = IBAN_.get_length();
	char pattern[ 2 ] = "\0";
	HString IBAN, tmpString;
	bool valid( false );
	do {
		if ( length < MIN_IBAN_LENGTH ) {
			_lastErrorMessage_.assign( "IBAN: Number too short (" ).append( static_cast<int>( length ) ).append( ")." );
			break;
		}
		IBAN.reserve( length );
		for ( int i( 0 ); i < length; ++ i ) {
			if ( is_alnum( IBAN_[ i ] ) ) {
				IBAN += IBAN_[ i ];
			}
		}
		length = IBAN.get_length();
		if ( length < MIN_IBAN_LENGTH ) {
			_lastErrorMessage_.assign( "IBAN: Number too short (" ).append( static_cast<int>( length ) ).append( ")." );
			break;
		}
		if ( ! ( is_alpha( IBAN[ 0 ] ) && is_alpha( IBAN[ 1 ] ) ) ) {
			_lastErrorMessage_ = "IBAN: No country code present.";
			break;
		}
		tmpString = IBAN.left( 4 );
		IBAN.shift_left ( 4 );
		IBAN += tmpString;
	/*	M_LOG ( IBAN ); */
		IBAN.lower();
		for ( int i( 0 ); i < length; ++ i ) {
			if ( is_alpha( IBAN[ i ] ) ) {
				static int const DIGIT_PAIR_BUF_SIZE( 3 );
				char digitPairBuf[DIGIT_PAIR_BUF_SIZE];
				snprintf( digitPairBuf, DIGIT_PAIR_BUF_SIZE, "%02d", ( IBAN[ i ].get() - 'a' ) + 10 );
				pattern[ 0 ] = static_cast<char>( IBAN[ i ].get() );
				IBAN.replace( pattern, digitPairBuf );
				length = IBAN.get_length();
			}
		}
	/*	M_LOG ( IBAN ); */
		int sum( modulo_ASCII( IBAN, 97 ) );
		if ( sum != 1 ) {
			_lastErrorMessage_.assign( "IBAN: bad checksum: " ).append( sum );
			break;
		}
		valid = true;
	} while ( false );
	return valid;
	M_EPILOG
}

}

}

}

