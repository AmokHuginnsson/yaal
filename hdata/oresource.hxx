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
#define YAAL_HDATA_ORESOURCE_HXX_INCLUDED 1

#include "hcore/harray.hxx"
#include "hconsole/hcontrol.hxx"

namespace yaal {

/*! \brief Api for rapid database based aplication development.
 * 
 * This namspace joins powers of APIs that are found in hconsole
 * and in dbwrapper. Its user may rapidly create fulfeatured applications
 * that use console interface, mouse capabilities and database access.
 */
namespace hdata {

/*! \brief Database connected TUI controls flags and configuration bits.
 */
struct DATACONTROL_BITS {
/*! \brief control role flags
 */
	struct ROLE {
		/*! \brief the flags
		 */
		typedef enum {
			INVALID,
			MAIN,		/*!< main control of a window, contents of this control holds
										 data of main record-set of a window */

			FILTER,	/*!< this kind of control is meant to setup filter for main
										control for `view mode' */
			DATA		/*!< this kind of control is used to store record-set data in `edit mode' */
		} role_t;
	};
/*! \brief control types
 */
	struct TYPE {
		typedef enum {
			INVALID,
			EDIT,
			LIST,
			TREE,
			COMBO,
			DATE,
			CHECK
		} type_t; 
	};
};

/*! \brief TUI control attributes description.
 */
struct OAttributes {
	bool	_drawLabel;					/*!< should be label driven */
	int		_disabledAttribute;	/*!< attribute of control in disabled state */
	int		_enabledAttribute;	/*!< attribute of control in enabled state */
	int		_focusedAttribute;	/*!< attribute of control in focused state */
};

/*! \brief Description of HListControl column.
 */
struct OColumnInfo {
	int _placement;    /*!< what place should this new column take */
	char const* _name; /*!< column name */
	int _width;        /*!< column width */
	yaal::hconsole::HControl::BITS::ALIGN::align_t _align;	/*!< column alignment */
	type_id_t _type;   /*!< data type stored in that column */
	OColumnInfo( void )
		: _placement( 0 ), _name( NULL ), _width( 0 ),
		_align( yaal::hconsole::HControl::BITS::ALIGN::LEFT ),
		_type( TYPE::HSTRING ) {}
};

class HDataControl;

/*! \brief Text mode user interface binary description.
 */
struct OResource {
/* data part */
	char const*	_table;   /*!< name of table in database asociated with control */
	char const*	_columns; /*!< what columns from record-set we are looking for */
	char const* _id;      /*!< ID column name */
	char const*	_filter;  /*!< additional filter for record-set (WHERE clause) */
	char const*	_sort;    /*!< additional sort for record-set (ORDER BY clause) */
/* control part */
	int			_row;								/*!< top coordinate of control */
	int			_column;						/*!< left coordinate of control */
	int			_height;						/*!< height of control */
	int			_width;							/*!< width of control */
	char const*	_label;			/*!< control label */
	OAttributes*	_attributes;	/*!< control's attributes */
/* relative functionality configuration options */
	DATACONTROL_BITS::ROLE::role_t	_role;		/*!< tell how control should be treaded */
	DATACONTROL_BITS::TYPE::type_t	_type;		/*!< predefined type of control */
	void*	_typeSpecific;				/*!< additional initialization values control
																		 type specific */
	OColumnInfo* _columnInfo;	/*!< if this control is a child control this field
																	 holds initialization information about
																	 parent's record's field-column */
	int			_parent;						/*!< index of parent control (tree data structure) */
};

/*! \brief edit control type specific data
 */
struct OEditControlResource {
	int			_maxStringSize;		/*!< maximum length of contained string */
	char const*	_value;			/*!< initial value for control */
	char const*	_mask;				/*!< tell what can be inserted into control */
	bool		_replace;					/*!< is control in replace mode? */
	bool		_multiLine;				/*!< is this control a multiline edit? */
	bool		_readOnly;				/*!< should editing be disabled ? */
	bool		_rightAligned;		/*!< is control content right aligned */
	bool		_password;				/*!< is control in password mode? (no echo) */
	int			_maxHistoryLevel;	/*!< history buffer size */
};

/*! \brief list control type specific data
 */
struct OListControlResource {
	bool	_checkable;					/*!< can control items be checked/unchecked */
	bool	_sortable;					/*!< is control content sortable */
	bool	_searchable;				/*!< can be item searched */
	bool	_editable;
	bool	_drawHeader;				/*!< should be header droven */
};

typedef yaal::hcore::HArray<OResource> resources_t;

}

}

#endif /* #ifndef YAAL_HDATA_ORESOURCE_HXX_INCLUDED */

