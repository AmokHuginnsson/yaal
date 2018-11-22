/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HSTRINGSTREAM_HXX_INCLUDED
#define YAAL_TOOLS_HSTRINGSTREAM_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

/*! \brief Perform stream operations on character string.
 */
class HStringStream : public yaal::hcore::HStreamInterface {
public:
	typedef HStringStream this_type;
	typedef yaal::hcore::HStreamInterface base_type;
private:
	mutable bool _used; /* FIXME Confirm mutable is necessary here. */
	mutable int long _offset;
	mutable yaal::hcore::HString _buffer;
	mutable yaal::hcore::HUTF8String _utf8;
public:
	HStringStream( void );
	HStringStream( yaal::hcore::HString const& );
	yaal::hcore::HString const& string( void ) const;
	void set_buffer( yaal::hcore::HString const& );
	yaal::hcore::HString const& str( void ) const;
	void str( yaal::hcore::HString const& );
	void use( void ) const;
	yaal::hcore::HString const& consume( void ) const;
	bool is_empty( void ) const;
private:
	virtual int long do_write( void const*, int long ) override;
	virtual int long do_read( void*, int long ) override;
	virtual void do_reset( void ) override;
	virtual void do_flush( void ) override;
	virtual bool do_is_valid( void ) const override;
	virtual POLL_TYPE do_poll_type( void ) const override;
	virtual void const* do_data( void ) const override;
	HStringStream( HStringStream const& ) = delete;
	HStringStream& operator = ( HStringStream const& ) = delete;
};

yaal::hcore::HString const& operator << ( yaal::hcore::HStreamInterface const&, HStringStream const& );

}

}

#endif /* #ifndef YAAL_TOOLS_HSTRINGSTREAM_HXX_INCLUDED */

