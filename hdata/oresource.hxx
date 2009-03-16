/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	oresource.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_HDATA_ORESOURCE_HXX_INCLUDED
#define YAAL_HDATA_ORESOURCE_HXX_INCLUDED

#include "hconsole/hcontrol.hxx"

namespace yaal
{

/*! \brief Api for rapid database based aplication development.
 * 
 * This namspace joins powers of APIs that are found in hconsole
 * and in dbwrapper. Its user may rapidly create fulfeatured applications
 * that use console interface, mouse capabilities and database access.
 */
namespace hdata
{

#define D_VCSID_ORESOURCE_H "$Id$"

/*! \brief Database connected TUI controls flags and configuration bits.
 */
struct DATACONTROL_BITS
	{
/*! \brief control role flags
 */
	struct ROLE
		{
		/*! \brief the flags
		 */
		typedef enum
			{
			D_INVALID,
			D_MAIN,		/*!< main control of a window, contents of this control holds
										 data of main record-set of a window */

			D_FILTER,	/*!< this kind of control is meant to setup filter for main
										control for `view mode' */
			D_DATA		/*!< this kind of control is used to store record-set data in `edit mode' */
			} role_t;
		};
/*! \brief control types
 */
	struct TYPE
		{
		typedef enum
			{
			D_INVALID,
			D_EDIT,
			D_LIST,
			D_TREE,
			D_COMBO,
			D_DATE,
			D_CHECK
			} type_t; 
		};
	};

/*! \brief TUI control attributes description.
 */
struct OAttributes
	{
	bool	f_bDrawLabel;					/*!< should be label driven */
	int		f_iDisabledAttribute;	/*!< attribute of control in disabled state */
	int		f_iEnabledAttribute;	/*!< attribute of control in enabled state */
	int		f_iFocusedAttribute;	/*!< attribute of control in focused state */
	};

/*! \brief Description of HListControl column.
 */
struct OColumnInfo
	{
	int f_iPlacement; 			/*!< what place should this new column take */
	char const* f_pcName;	/*!< column name */
	int f_iWidth;						/*!< column width */
	yaal::hconsole::HControl::BITS::ALIGN::align_t f_eAlign;	/*!< column alignment */
	type_t f_eType;					/*!< data type stored in that column */
	OColumnInfo( void )
		: f_iPlacement( 0 ), f_pcName( NULL ), f_iWidth( 0 ),
		f_eAlign( yaal::hconsole::HControl::BITS::ALIGN::D_LEFT ),
		f_eType( TYPE::D_HSTRING ) {}
	};

class HDataControl;

/*! \brief Text mode user interface binary description.
 */
struct OResource
	{
/* data part */
	char const*	f_pcTable;   /*!< name of table in database asociated with control */
	char const*	f_pcColumns; /*!< what columns from record-set we are looking for */
	char const* f_pcId;      /*!< ID column name */
	char const*	f_pcFilter;  /*!< additional filter for record-set (WHERE clause) */
	char const*	f_pcSort;    /*!< additional sort for record-set (ORDER BY clause) */
/* control part */
	int			f_iRow;								/*!< top coordinate of control */
	int			f_iColumn;						/*!< left coordinate of control */
	int			f_iHeight;						/*!< height of control */
	int			f_iWidth;							/*!< width of control */
	char const*	f_pcLabel;			/*!< control label */
	OAttributes*	f_psAttributes;	/*!< control's attributes */
/* relative functionality configuration options */
	DATACONTROL_BITS::ROLE::role_t	f_eRole;		/*!< tell how control should be treaded */
	DATACONTROL_BITS::TYPE::type_t	f_eType;		/*!< predefined type of control */
	void*	f_pvTypeSpecific;				/*!< additional initialization values control
																		 type specific */
	OColumnInfo* f_psColumnInfo;	/*!< if this control is a child control this field
																	 holds initialization information about
																	 parent's record's field-column */
	int			f_iParent;						/*!< index of parent control (tree data structure) */
	};

/*! \brief edit control type specific data
 */
struct OEditControlResource
	{
	int			f_iMaxStringSize;		/*!< maximum length of contained string */
	char const*	f_pcValue;			/*!< initial value for control */
	char const*	f_pcMask;				/*!< tell what can be inserted into control */
	bool		f_bReplace;					/*!< is control in replace mode? */
	bool		f_bMultiLine;				/*!< is this control a multiline edit? */
	bool		f_bReadOnly;				/*!< should editing be disabled ? */
	bool		f_bRightAligned;		/*!< is control content right aligned */
	bool		f_bPassword;				/*!< is control in password mode? (no echo) */
	int			f_iMaxHistoryLevel;	/*!< history buffer size */
	};

/*! \brief list control type specific data
 */
struct OListControlResource
	{
	bool	f_bCheckable;					/*!< can control items be checked/unchecked */
	bool	f_bSortable;					/*!< is control content sortable */
	bool	f_bSearchable;				/*!< can be item searched */
	bool	f_bEditable;
	bool	f_bDrawHeader;				/*!< should be header droven */
	};

}

}

#endif /* not YAAL_HDATA_ORESOURCE_HXX_INCLUDED */
