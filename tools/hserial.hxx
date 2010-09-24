/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hserial.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HSERIAL_HXX_INCLUDED
#define YAAL_TOOLS_HSERIAL_HXX_INCLUDED

#include "hcore/hstring.hxx"
#include "hcore/hchunk.hxx"
#include "hcore/hrawfile.hxx"
#include "hcore/hstrongenum.hxx"

namespace yaal
{

namespace tools
{

/*! \brief Serial port handling class.
 */
class HSerial : public hcore::HRawFile
	{
protected:
	typedef HSerial this_type;
	typedef HRawFile base_type;
public:
	/*! \brief Serial port configuration flags.
	 */
	struct FLAG
		{
		typedef enum
			{
			DEFAULT								= 0x0001,
			FLOW_CONTROL_HARDWARE = 0x0002,
			FLOW_CONTROL_SOFTWARE = 0x0004,
			STOP_BITS_1						= 0x0008,
			STOP_BITS_2						= 0x0010,
			PARITY_CHECK					= 0x0020,
			PARITY_ODD						= 0x0040,
			BITS_PER_BYTE_8				= 0x0080,
			BITS_PER_BYTE_7				= 0x0100,
			BITS_PER_BYTE_6				= 0x0200,
			BITS_PER_BYTE_5				= 0x0400,
			CANONICAL							= 0x0800,
			ECHO									= 0x1000,
			CR2NL									= 0x2000
			} enum_t;
		};
	typedef yaal::hcore::HStrongEnum<FLAG> flag_t;
	/*! \brief Serial port data trnsmission speeds.
	 */
	struct SPEED
		{
		typedef enum
			{
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
			} enum_t;
		};
	typedef yaal::hcore::HStrongEnum<SPEED> speed_t;
protected:
	speed_t _speed;
	flag_t _flags;
	hcore::HString _devicePath;
	hcore::HChunk _tIO;
	hcore::HChunk _backUpTIO;
public:
	HSerial( yaal::hcore::HString const& = yaal::hcore::HString() ); /* device path */
	virtual ~HSerial( void );
	bool open( void );
	void set_speed( speed_t );
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

#endif /* not YAAL_TOOLS_HSERIAL_HXX_INCLUDED */

