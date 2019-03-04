/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file tools/huginn/blob.hxx
 * \brief Declaration of huginn::HBlob class.
 */

#ifndef YAAL_TOOLS_HUGINN_BLOB_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_BLOB_HXX_INCLUDED 1

#include "tools/huginn/value.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HBlob : public HValue {
public:
	typedef HBlob this_type;
	typedef HValue base_type;
	typedef yaal::hcore::HChunk value_type;
private:
	value_type _data;
public:
	HBlob( HClass const*, yaal::hcore::HChunk&& );
	int long get_size( void ) const;
	value_type const& value( void ) const;
private:
	virtual value_t do_clone( huginn::HThread*, HHuginn::value_t*, int ) const override;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_BLOB_HXX_INCLUDED */

