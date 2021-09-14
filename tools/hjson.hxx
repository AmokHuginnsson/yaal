/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/hjson.hxx
 * \brief Declaration HJSON class.
 */

#ifndef YAAL_TOOLS_HJSON_HXX_INCLUDED
#define YAAL_TOOLS_HJSON_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"
#include "tools/model.hxx"

namespace yaal {

namespace tools {

class HJSON {
public:
	typedef HJSON this_type;
	enum class PARSER {
		STRICT,
		RELAXED
	};
private:
	model::HValue _element;
public:
	HJSON( void );
	HJSON( HJSON const& ) = default;
	HJSON( HJSON&& ) = default;
	HJSON& operator = ( HJSON const& ) = default;
	HJSON& operator = ( HJSON&& ) = default;
	bool operator == ( HJSON const& ) const;
	bool operator != ( HJSON const& other_ ) const {
		return ( ! operator == ( other_ ) );
	}
	model::HValue& element( void );
	void load( yaal::hcore::HStreamInterface&, PARSER = PARSER::STRICT );
	void save( yaal::hcore::HStreamInterface&, bool = true ) const;
	void clear( void );
};

typedef yaal::hcore::HExceptionT<HJSON> HJSONException;

}

}

#endif /* #ifndef YAAL_TOOLS_HJSON_HXX_INCLUDED */

