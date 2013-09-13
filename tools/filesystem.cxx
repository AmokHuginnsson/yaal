/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	filesystem.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "filesystem.hxx"
#include "hfsitem.hxx"

namespace yaal {

namespace tools {

namespace filesystem {

find_result find( yaal::hcore::HString const& in, yaal::hcore::HRegex const& pattern_,
		int minDepth_, int maxDepth_, FILE_TYPE::enum_t fileType_ ) {
	find_result result;
	HFSItem p( in );
	if ( !! p ) {
		if ( p.is_file() ) {
			if ( ( minDepth_ == 0 ) && ( fileType_ & FILE_TYPE::REGULAR_FILE ) && pattern_.matches( in ) ) {
				result.push_back( in );
			}
		} else if ( p.is_directory() ) {
			for ( HFSItem::HIterator it( p.begin() ), end( p.end() ); it != end; ++ it ) {
				if ( it->is_file() ) {
					if ( ( minDepth_ == 0 ) && ( fileType_ & FILE_TYPE::REGULAR_FILE ) && pattern_.matches( it->get_path() ) )
						result.push_back( it->get_path() );
				} else if ( it->is_directory() ) {
					if ( ( minDepth_ == 0 ) && ( fileType_ & FILE_TYPE::DIRECTORY ) && pattern_.matches( it->get_path() ) )
						result.push_back( it->get_path() );
					if ( maxDepth_ > 0 ) {
						find_result sub( find( it->get_path(), pattern_, minDepth_ > 0 ? minDepth_ - 1 : 0, maxDepth_ - 1 ) );
						result.insert( result.end(), sub.begin(), sub.end() );
					}
				}
			}
		}
	}
	return ( result );
}

}

}

}

