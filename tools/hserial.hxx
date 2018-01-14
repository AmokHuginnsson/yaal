/* Read yaal/LICENSE.md file for copyright and licensing information. */

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
		static M_YAAL_TOOLS_PUBLIC_API flag_t const DEFAULT;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const FLOW_CONTROL_HARDWARE;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const FLOW_CONTROL_SOFTWARE;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const STOP_BITS_1;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const STOP_BITS_2;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const PARITY_CHECK;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const PARITY_ODD;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const BITS_PER_BYTE_8;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const BITS_PER_BYTE_7;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const BITS_PER_BYTE_6;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const BITS_PER_BYTE_5;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const CANONICAL;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const ECHO;
		static M_YAAL_TOOLS_PUBLIC_API flag_t const CR2NL;
	};
	/*! \brief Serial port data transmission speeds.
	 */
	enum class SPEED {
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
	};
protected:
	SPEED _speed;
	flag_t _flags;
	hcore::HString _devicePath;
	hcore::HChunk _tIO;
	hcore::HChunk _backUpTIO;
public:
	HSerial( yaal::hcore::HString const& = yaal::hcore::HString() ); /* device path */
	virtual ~HSerial( void );
	void open( void );
	void set_speed( SPEED );
	void set_flags( flag_t );
	void flush( int );
	void wait_for_eot( void );
	int timed_read( void*, int const, int );
	static flag_t FLAG_TEXT; /* flags for textmode operation */
protected:
	virtual int do_close( void ) override;
private:
	void compile( void );
	void compile_speed( void );
	void compile_flags( void );
};

typedef yaal::hcore::HExceptionT<HSerial, yaal::hcore::HRawFileException> HSerialException;

}

}

#endif /* #ifndef YAAL_TOOLS_HSERIAL_HXX_INCLUDED */

