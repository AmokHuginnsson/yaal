/* Read yaal/LICENSE.md file for copyright and licensing information. */

#ifndef YAAL_TOOLS_HEXPRESSION_HXX_INCLUDED
#define YAAL_TOOLS_HEXPRESSION_HXX_INCLUDED 1

#include "hcore/hstring.hxx"
#include "hcore/htree.hxx"
#include "hcore/harray.hxx"
#include "hcore/hchunk.hxx"

namespace yaal {

namespace tools {

/*! \brief Mathematical expression analyzer and evaluator.
 */
class HExpression final {
public:
	static int const MAX_VARIABLE_COUNT = 26;
	typedef HExpression this_type;
	enum class ERROR_CODE {
		OK,
		UNKNOWN_MNEMONIC,
		UNEXPECTED_TERMINATION,
		CLOSING_BRACKET_EXPECTED,
		CLOSING_ABSOLUTE_EXPECTED,
		CLOSING_FUNCTION_BRACKET_EXPECTED,
		OPENING_FUNCTION_BRACKET_EXPECTED,
		DIGIT_EXPECTED,
		UNEXPECTED_TOKEN,
		DOUBLE_SPACER,
		TRAILING_SPACER,
		LEADING_SPACER,
		PREMATURE_TERMINATION
	};
private:
	typedef hcore::HList<int> int_list_t;
	struct OEquationElement;
	typedef hcore::HTree<OEquationElement> tree_t;
	struct OEquationElement {
		typedef double long ( HExpression::* METHOD_t ) ( tree_t::const_node_t );
		METHOD_t METHOD;
		int_list_t _variables;
		OEquationElement() : METHOD(), _variables() {}
	};
	int _index;
	int _length;
	ERROR_CODE _error;
	double long _variables[ MAX_VARIABLE_COUNT ];
	typedef hcore::HArray<double long> constants_t;
	constants_t _constantsPool;
	typedef hcore::HArray<int> terminal_indexes_t;
	terminal_indexes_t _terminalIndexes;
	yaal::hcore::HChunk _formulaCache;
	tree_t _equationTree;
	char const* _formula;
public:
	/*{*/
	HExpression( void );
	HExpression( yaal::hcore::HString const& );
	HExpression( HExpression const& );
	HExpression& operator = ( HExpression const& );
	~HExpression( void );
	void swap( HExpression& );
	double long* compile( yaal::hcore::HString const& );
	double long* variables( void );
	double long& operator [] ( int );
	double long evaluate( void );
	char const* get_error( void ) const;
	int get_error_token( void ) const;
private:
	bool translate( yaal::hcore::HString const& );
	bool addition_production( tree_t::node_t );
	bool multiplication_production( tree_t::node_t );
	bool power_production( tree_t::node_t );
	bool signum_production( tree_t::node_t );
	bool factorial_production( tree_t::node_t );
	bool terminal_production( tree_t::node_t );
	double long count_branch( tree_t::const_node_t );
	double long addition( tree_t::const_node_t );
	double long multiplication( tree_t::const_node_t );
	double long power( tree_t::const_node_t );
	double long signum( tree_t::const_node_t );
	double long factorial( tree_t::const_node_t );
	double long bracket( tree_t::const_node_t );
	double long functions( tree_t::const_node_t );
	void shorten_the_branch( tree_t::node_t );
};

typedef yaal::hcore::HExceptionT<HExpression> HExpressionException;

void swap( yaal::tools::HExpression&, yaal::tools::HExpression& );

}

}

#endif /* #ifndef YAAL_TOOLS_HEXPRESSION_HXX_INCLUDED */

