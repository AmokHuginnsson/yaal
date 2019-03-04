/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/method.hxx
 * \brief Declaration of huginn::HClass::HMethod, huginn::HClass::HUnboundMethod and huginn::HClass::HBoundMethod classes.
 */

#ifndef YAAL_TOOLS_HUGINN_METHOD_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_METHOD_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HClass::HMethod : public HValue {
	typedef HClass::HMethod this_type;
	typedef HValue base_type;
protected:
	HHuginn::function_t _function;
public:
	HMethod( HClass const*, HHuginn::function_t const& );
	HHuginn::function_t const& function( void ) const {
		return ( _function );
	}
private:
	HMethod( HMethod const& ) = delete;
	HMethod& operator = ( HMethod const& ) = delete;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HClass::HUnboundMethod : public HValue {
	typedef HClass::HUnboundMethod this_type;
	typedef HValue base_type;
protected:
	HClass const* _juncture;
	HHuginn::function_t _function;
public:
	HUnboundMethod( HClass const*, HClass const*, HHuginn::function_t const& );
	HHuginn::value_t call( huginn::HThread*, HHuginn::values_t&, int );
private:
	HUnboundMethod( HUnboundMethod const& ) = delete;
	HUnboundMethod& operator = ( HUnboundMethod const& ) = delete;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

class HClass::HBoundMethod : public HValue {
	typedef HClass::HBoundMethod this_type;
	typedef HValue base_type;
private:
	HHuginn::function_t _function;
	HHuginn::value_t _objectHolder;
public:
	HBoundMethod( HClass const*, HHuginn::function_t const&, HHuginn::value_t const& );
	HHuginn::value_t call( huginn::HThread*, HHuginn::values_t&, int );
	HHuginn::value_t const& subject( void ) const {
		return ( _objectHolder );
	}
	HHuginn::function_t const& function( void ) const {
		return ( _function );
	}
private:
	HBoundMethod( HBoundMethod const& ) = delete;
	HBoundMethod& operator = ( HBoundMethod const& ) = delete;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_METHOD_HXX_INCLUDED */

