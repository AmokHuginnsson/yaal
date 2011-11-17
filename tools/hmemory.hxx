/*
---           yaal 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hmemory.hxx - this file is integral part of yaal project.

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

#ifndef YAAL_TOOLS_HMEMORY_HXX_INCLUDED
#define YAAL_TOOLS_HMEMORY_HXX_INCLUDED 1

#include "hcore/hstreaminterface.hxx"

namespace yaal {

namespace tools {

/*! \brief Stream based access to raw memory block.
 *
 * This stream based memory accessor wrapper does not own the memory block.
 */
class HMemory : public yaal::hcore::HStreamInterface {
public:
	typedef HMemory this_type;
	typedef yaal::hcore::HStreamInterface base_type;
	struct INITIAL_STATE {
		typedef enum {
			AUTO,
			VALID,
			INVALID
		} enum_t;
	};
private:
	void* _block;
	int long _size;
	int long _valid;
	int long _cursorRead;
	int long _cursorWrite;
public:
	/*! \brief Create new memory accessor.
	 *
	 * \param ptr - pointer to memory block to be wrapped.
	 * \param size - size of memory block in octets.
	 */
	HMemory( void* ptr, int long size, INITIAL_STATE::enum_t = INITIAL_STATE::AUTO );
	/*! \brief Copy constructor.
	 *
	 * \param src - original HMemory object to be copied.
	 */
	HMemory( HMemory const& src );
	/*! \brief Destructor.
	 */
	virtual ~HMemory( void );
	/*! \brief Assign operator.
	 *
	 * \param src - original HMemory object to be copied.
	 * \return Self.
	 */
	HMemory& operator = ( HMemory const& src );
	/*! \brief Check if two blocks of memory have identical contents.
	 *
	 * \param other - check for equality with this block.
	 * \return true iff other has same contents as this.
	 */
	bool operator == ( HMemory const& other ) const;
private:
	virtual int long do_write( void const* const, int long );
	virtual void do_flush( void ) const;
	virtual int long do_read( void* const, int long );
	virtual bool do_is_valid( void ) const;
};

typedef yaal::hcore::HExceptionT<HMemory, yaal::hcore::HStreamInterfaceException> HMemoryException;

}

}

#endif /* #ifndef YAAL_TOOLS_HMEMORY_HXX_INCLUDED */

