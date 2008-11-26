/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hrawdestructor.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_HCORE_HRAWDESTRUCTOR_H
#define __YAAL_HCORE_HRAWDESTRUCTOR_H

namespace yaal
{

namespace hcore
{

/*! \brief Automatic scope based custom destruction function to free resources.
 */
class HRawDestructor
	{
public:
	typedef void ( *destructor_t )( void* );
private:
	void* f_pvResourcePointer;
	destructor_t DESTRUCTOR;
public:
	HRawDestructor( void );
	HRawDestructor( void*, destructor_t );
	~HRawDestructor( void );
	void set_destruction_scheme( void*, destructor_t );
private:
	HRawDestructor( HRawDestructor const& );
	HRawDestructor& operator = ( HRawDestructor const& );
	};

}

}

#endif /* not __YAAL_HCORE_HRAWDESTRUCTOR_H */

