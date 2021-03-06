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
	virtual HHuginn::value_t do_operator_call( huginn::HThread*, HHuginn::value_t&, HHuginn::values_t&, int ) override;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
	virtual bool do_operator_equals( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_less( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_greater( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_less_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual bool do_operator_greater_or_equal( HThread*, HHuginn::value_t const&, HHuginn::value_t const&, int ) const override;
	virtual hash_value_t do_operator_hash( HThread*, HHuginn::value_t const&, int ) const override;
	virtual yaal::hcore::HString do_code( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
	virtual yaal::hcore::HString do_to_string( HThread*, HHuginn::value_t const&, HCycleTracker&, int ) const override;
};

class HPartial : public HValue {
public:
	typedef HPartial this_type;
	typedef HValue base_type;
	class HIndexMap {
		int _internalIndex;
		int _externalIndex;
	public:
		HIndexMap( int internalIndex_, int externalIndex_ )
			: _internalIndex( internalIndex_ )
			, _externalIndex( externalIndex_ ) {
		}
		bool operator != ( int internalIndex_ ) const {
			return ( internalIndex_ != _internalIndex );
		}
		int internal_index( void ) const {
			return ( _internalIndex );
		}
		int external_index( void ) const {
			return ( _externalIndex );
		}
		void shift( int by_ ) {
			_internalIndex += by_;
		}
	};
	typedef yaal::hcore::HArray<HIndexMap> unbound_indexes_t;
private:
	HHuginn::value_t _callable;
	HHuginn::values_t _arguments;
	HHuginn::values_t _cache;
	unbound_indexes_t _unboundIndexes;
	int _argumentCount;
public:
	HPartial( HClass const*, HHuginn::value_t const&, HHuginn::values_t&&, unbound_indexes_t const&, int );
private:
	virtual HHuginn::value_t do_operator_call( huginn::HThread*, HHuginn::value_t&, HHuginn::values_t&, int ) override;
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
private:
	HPartial( HPartial const& ) = delete;
	HPartial& operator = ( HPartial const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_FUNCTIONREFERENCE_HXX_INCLUDED */

