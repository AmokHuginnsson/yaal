/* Read yaal/LICENSE.md file for copyright and licensing information. */

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

#ifndef YAAL_TOOLS_HUGINN_INTROFUNCTION_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_INTROFUNCTION_HXX_INCLUDED 1

#include "function.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HIntroFunction : public HFunction {
public:
	typedef HIntroFunction this_type;
	typedef HHuginn::expressions_t expressions_t;
	typedef void ( huginn::HThread::* function_frame_creator_t )( HStatement const*, HHuginn::value_t*, int );
	typedef void ( huginn::HThread::* function_frame_popper_t )( void );
public:
	HIntroFunction( HHuginn::identifier_id_t, parameter_names_t const&, HHuginn::scope_t const&, expressions_t const&, bool, bool );
	HHuginn::value_t execute( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int ) const;
	HHuginn::value_t execute_incremental_main( huginn::HThread*, HHuginn::value_t*, HHuginn::values_t&, int ) const;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_INTROFUNCTION_HXX_INCLUDED */

