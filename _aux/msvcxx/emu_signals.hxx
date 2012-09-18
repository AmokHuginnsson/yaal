#ifndef YAAL_MSVCXX_EMU_SIGNALS_HXX_INCLUDED
#define YAAL_MSVCXX_EMU_SIGNALS_HXX_INCLUDED 1

#include "hcore/htls.hxx"

namespace msvcxx {

class SignalsSetup {
	sigset_t _mask;
	HANDLE _interrupt;
public:
	SignalsSetup( void );
	~SignalsSetup( void );
	void set_mask( int, sigset_t const* );
	sigset_t get_mask( void ) const;
	bool is_blocked( int ) const;
	HANDLE interrupt( void );
	void signal( void );
};

typedef yaal::hcore::HTLS<SignalsSetup> TLSSignalsSetup;
extern TLSSignalsSetup _tlsSignalsSetup_;

} /* namespace msvcxx */

#endif /* #ifndef YAAL_MSVCXX_EMU_SIGNALS_HXX_INCLUDED */
