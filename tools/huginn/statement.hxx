/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/huginn/statment.hxx
 * \brief Declaration of huginn::HStatement class.
 */

#ifndef YAAL_TOOLS_HUGINN_STATEMENT_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_STATEMENT_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/htaggedpod.hxx"
#include "tools/hhuginn.hxx"
#include "tools/huginn/thread.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HStatement {
public:
	typedef HStatement this_type;
private:
	HHuginn::statement_id_t _id;
	int _fileId;
	executing_parser::HRange _range;
public:
	HStatement( HHuginn::statement_id_t id_, int fileId_, executing_parser::range_t range_ )
		: _id( id_ )
		, _fileId( fileId_ )
		, _range( range_ ) {
		return;
	}
	virtual ~HStatement( void ) {
		return;
	}
	void execute( HThread* thread_ ) const {
		if ( _isKilled_ ) {
			throw HHuginn::HHuginnRuntimeException( "Killed interior.", _fileId, _range.start() );
		}
		do_execute( thread_ );
		return;
	}
	HHuginn::statement_id_t id( void ) const {
		return ( _id );
	}
	executing_parser::range_t const& range( void ) const {
		return ( _range );
	}
	int position( void ) const {
		return ( _range.start() );
	}
	int file_id( void ) const {
		return ( _fileId );
	}
	void set_range( int fileId_, executing_parser::range_t range_ ) {
		_fileId = fileId_;
		_range = range_;
		return;
	}
protected:
	virtual void do_execute( HThread* ) const {}
};

extern HHuginn::statement_id_t const INVALID_STATEMENT_IDENTIFIER;
static int const INVALID_POSITION = -1;

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_STATEMENT_HXX_INCLUDED */

