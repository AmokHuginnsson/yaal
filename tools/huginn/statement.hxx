/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  statement.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

/* YAAL_PRIVATE_IMPLEMENTATION_DETAIL */

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
			throw HHuginn::HHuginnRuntimeException( "Killed interior.", _position );
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

