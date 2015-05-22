/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  thread.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HUGINN_THREAD_HXX_INCLUDED
#define YAAL_TOOLS_HUGINN_THREAD_HXX_INCLUDED 1

#include "tools/huginn/frame.hxx"

namespace yaal {

namespace tools {

namespace huginn {

class HThread {
public:
	typedef HThread this_type;
	typedef yaal::hcore::HPointer<huginn::HFrame> frame_t;
	typedef yaal::hcore::HStack<frame_t> frames_t;
private:
	frames_t _frames;
	yaal::hcore::HThread::id_t _id;
	HHuginn* _huginn;
	yaal::hcore::HString _exceptionMessage;
	int _exceptionPosition;
public:
	HThread( HHuginn*, yaal::hcore::HThread::id_t );
	void create_function_frame( void );
	void create_loop_frame( void );
	void create_scope_frame( void );
	void pop_frame( void );
	HFrame* current_frame( void );
	HFrame const* current_frame( void ) const;
	void break_execution( HFrame::STATE, HHuginn::value_t const& = HHuginn::value_t(), int = 0 );
	void set_exception( yaal::hcore::HString const&, int );
	bool can_continue( void ) const;
	yaal::hcore::HThread::id_t id( void ) const;
	void flush_exception( void );
	bool has_exception( void ) const;
	HHuginn& huginn( void );
private:
	HThread( HThread const& ) = delete;
	HThread& operator = ( HThread const& ) = delete;
};

}

}

}

#endif /* #ifndef YAAL_TOOLS_HUGINN_THREAD_HXX_INCLUDED */

