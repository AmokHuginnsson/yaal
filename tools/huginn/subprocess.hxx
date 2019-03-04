/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_SUBPROCESS_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_SUBPROCESS_HXX_INCLUDED 1

#include "tools/hpipedchild.hxx"
#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HSubprocess : public HValue {
private:
	HPipedChild _pipedChild;
public:
	typedef yaal::hcore::HStreamInterface::ptr_t ( HPipedChild::* stream_getter_t )( void );
	HSubprocess( HClass const*, HHuginn::values_t& );
	static HHuginn::value_t is_alive( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t get_pid( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t wait( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t kill( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t stream( char const*, stream_getter_t, huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime*, HClass const* );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override __attribute__((noreturn));
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_SUBPROCESS_HXX_INCLUDED */

