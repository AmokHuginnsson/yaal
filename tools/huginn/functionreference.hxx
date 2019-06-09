/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/functionreference.hxx
 * \brief Declaration of huginn::HFunctionReference class.
 */

#ifndef YAAL_TOOLS_HUGINN_FUNCTIONREFERENCE_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_FUNCTIONREFERENCE_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HFunctionReference : public HValue {
	typedef HFunctionReference this_type;
	typedef HValue base_type;
private:
	HHuginn::identifier_id_t _identifierId;
	/*! \brief Function definition holder.
	 *
	 * It has to be a ref count bumping pointer because lambdas are stored
	 * with help of HFunctionReference directly in run-time,
	 * so this holder must have shared ownership.
	 */
	HHuginn::function_t _function;
	HClass const* _juncture;
	yaal::hcore::HString _doc;
public:
	HFunctionReference( HClass const*, HHuginn::identifier_id_t, HHuginn::function_t const&, yaal::hcore::HString const&, HClass const* );
	HHuginn::identifier_id_t const& identifier_id( void ) const {
		return ( _identifierId );
	}
	HHuginn::function_t const& function( void ) const {
		return ( _function );
	}
	HClass const* juncture( void ) const {
		return ( _juncture );
	}
	void reset( HHuginn::function_t const& );
	yaal::hcore::HString const& doc( void ) const;
private:
	HFunctionReference( HFunctionReference const& ) = delete;
	HFunctionReference& operator = ( HFunctionReference const& ) = delete;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FUNCTIONREFERENCE_HXX_INCLUDED */

