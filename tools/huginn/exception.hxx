/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_EXCEPTION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_EXCEPTION_HXX_INCLUDED 1

#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

namespace exception {

class HStackFrameInfo : public HHuginn::HValue {
private:
	HHuginn::HCallSite _callSite;
public:
	HStackFrameInfo( HHuginn::HClass const*, HHuginn::HCallSite const& );
	static HHuginn::value_t file( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t line( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t column( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t context( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::value_t to_string( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int );
	static HHuginn::class_t get_class( HRuntime* );
private:
	virtual HHuginn::value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override __attribute__((noreturn));
};

HHuginn::class_t get_class( HRuntime* );

HHuginn::class_t create_class(
	HRuntime*,
	yaal::hcore::HString const&,
	yaal::hcore::HString const&,
	HHuginn::VISIBILITY = HHuginn::VISIBILITY::PACKAGE,
	HHuginn::HClass const* = nullptr
);

}

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_EXCEPTION_HXX_INCLUDED */

