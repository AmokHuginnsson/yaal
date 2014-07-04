/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hserial.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HSERIAL_HXX_INCLUDED
#define YAAL_TOOLS_HSERIAL_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hrawfile.hxx"
#include "hcore/hbitflag.hxx"

namespace yaal {

namespace tools {

/*! \brief Serial port handling class.
 */
class HSerial : public hcore::HRawFile {
public:
	typedef HSerial this_type;
	typedef HRawFile base_type;
	/*! \brief Serial port configuration flags.
	 */
	struct FLAG;
	typedef yaal::hcore::HBitFlag<FLAG> flag_t;
	struct FLAG {
		static flag_t const DEFAULT;
		static flag_t const FLOW_CONTROL_HARDWARE;
		static flag_t const FLOW_CONTROL_SOFTWARE;
		static flag_t const STOP_BITS_1;
		static flag_t const STOP_BITS_2;
		static flag_t const PARITY_CHECK;
		static flag_t const PARITY_ODD;
		static flag_t const BITS_PER_BYTE_8;
		static flag_t const BITS_PER_BYTE_7;
		static flag_t const BITS_PER_BYTE_6;
		static flag_t const BITS_PER_BYTE_5;
		static flag_t const CANONICAL;
		static flag_t const ECHO;
		static flag_t const CR2NL;
	};
	/*! \brief Serial port data trnsmission speeds.
	 */
	struct SPEED {
		typedef enum {
			DEFAULT,
			B_230400,
			B_115200,
#if defined( HAVE_DECL_B76800 ) && ( HAVE_DECL_B76800 == 1 )
			B_76800,
#endif /* HAVE_DECL_B76800 */
			B_57600,
			B_38400,
#if defined( HAVE_DECL_B28800 ) && ( HAVE_DECL_B28800 == 1 )
			B_28800,
#endif /* HAVE_DECL_B28800 */
			B_19200,
#if defined( HAVE_DECL_B14400 ) && ( HAVE_DECL_B14400 == 1 )
			B_14400,
#endif /* HAVE_DECL_B14400 */
			B_9600,
#if defined( HAVE_DECL_B7200 ) && ( HAVE_DECL_B7200 == 1 )
			B_7200,
#endif /* HAVE_DECL_B7200 */
			B_4800,
			B_2400
		} speed_t;
	};
protected:
	SPEED::speed_t _speed;
	flag_t _flags;
	hcore::HString _devicePath;
	hcore::HChunk _tIO;
	hcore::HChunk _backUpTIO;
public:
	HSerial( yaal::hcore::HString const& = yaal::hcore::HString() ); /* device path */
	virtual ~HSerial( void );
	void open( void );
	void set_speed( SPEED::speed_t );
	void set_flags( flag_t );
	void flush( int );
	void wait_for_eot( void );
	int timed_read( void* const, int const, int );
	static flag_t FLAG_TEXT; /* flags for textmode operation */
protected:
	virtual int do_close( void );
private:
	void compile( void );
	void compile_speed( void );
	void compile_flags( void );
};

typedef yaal::hcore::HExceptionT<HSerial, yaal::hcore::HRawFileException> HSerialException;

}

}

#endif /* #ifndef YAAL_TOOLS_HSERIAL_HXX_INCLUDED */

