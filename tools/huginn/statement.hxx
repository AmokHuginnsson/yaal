/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/huginn/statment.hxx
 * \brief Declaration of huginn::HStatement class.
 */

#ifndef YAAL_TOOLS_HUGINN_STATEMENT_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_STATEMENT_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/htaggedpod.hxx"
#include "tools/hhuginn.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HThread;

class HStatement {
public:
	typedef HStatement this_type;
	struct statement_tag;
	typedef yaal::hcore::HTaggedPOD<int, statement_tag> statement_id_t;
private:
	statement_id_t _id;
	int _fileId;
	int _position;
public:
	HStatement( statement_id_t id_, int fileId_, int position_ )
		: _id( id_ )
		, _fileId( fileId_ )
		, _position( position_ ) {
		return;
	}
	virtual ~HStatement( void ) {
		return;
	}
	void execute( HThread* thread_ ) const {
		if ( _isKilled_ ) {
			throw HHuginn::HHuginnRuntimeException( "Killed interior.", _fileId, _position );
		}
		do_execute( thread_ );
		return;
	}
	statement_id_t id( void ) const {
		return ( _id );
	}
	int position( void ) const {
		return ( _position );
	}
	int file_id( void ) const {
		return ( _fileId );
	}
	void set_position( int fileId_, int position_ ) {
		_fileId = fileId_;
		_position = position_;
		return;
	}
protected:
	virtual void do_execute( HThread* ) const {}
};

extern HStatement::statement_id_t const INVALID_STATEMENT_IDENTIFIER;
static int const INVALID_FILE_ID = -1;
static int const INVALID_POSITION = -1;
static int const MAIN_FILE_ID = 0;

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_STATEMENT_HXX_INCLUDED */

