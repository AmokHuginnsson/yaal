/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdatabase.h - this file is integral part of `stdhapi' project.

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

#ifndef __DBWRAPPER_HDATABASE_H
#define __DBWRAPPER_HDATABASE_H

namespace stdhapi
{

namespace dbwrapper
{

class HDataBase
	{
private:
	/*{*/
	void * f_pvCoreData;	/* very internal for this class used only in base cla */
	/*}*/
protected:
	/*{*/
	void * f_pvLastResult; /* result of last query, helps prevent requering
														with out retriving/freeing old one */
	/*}*/
public:
	/*{*/
	HDataBase ( void );
	virtual ~HDataBase ( void );
	int login ( char const *,		/* database */
							char const *,		/* login */
							char const * );	/* password */
	long int query ( char const * );
	void free_result ( void );
	void * get_result ( void );
	long int insert_id ( void );
	char const * get_error ( void ) const;
	/*}*/
protected:
	/*{*/
	/*}*/
private:
	/*{*/
	HDataBase ( const HDataBase & );
	HDataBase & operator = ( const HDataBase & );
	/*}*/
	};

}

}

#endif /* not __DBWRAPPER_HDATABASE_H */
