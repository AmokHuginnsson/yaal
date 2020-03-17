#include <accctrl.h>
#include <aclapi.h>

#include "emu_pwd.hxx"

namespace msvcxx {

typedef int uid_t;

void recode( uid_t& uid_, PSID psid_, int nth_ ) {
	int mask[] = { 7, 7, 255, 4095 };
	int shift[] = { 26, 23, 15, 0 };
	int block( *GetSidSubAuthority( psid_, nth_ ) );
	uid_ |= ( ( block & mask[nth_] ) << shift[nth_] );
}

uid_t recode_sid( PSID psid_ ) {
	uid_t uid( 0 );
	int sidBlockCount( *GetSidSubAuthorityCount( psid_ ) );
	uid = *GetSidSubAuthority( psid_, sidBlockCount - 1 );
	if ( sidBlockCount >= 5 ) {
	} else if ( sidBlockCount > 2 ) {
		uid = ( sidBlockCount << 29 );
		recode( uid, psid_, 0 );
		if ( sidBlockCount > 1 ) {
			recode( uid, psid_, 1 );
		}
		if ( sidBlockCount > 2 ) {
			recode( uid, psid_, 2 );
		}
		if ( sidBlockCount > 3 ) {
			recode( uid, psid_, 3 );
		}
		uid = -uid;
	} else if ( sidBlockCount > 1 ) {
		int block( *GetSidSubAuthority( psid_, 0 ) );
		if ( block < 16 ) {
			uid |= ( block << 10 );
		}
		uid |= ( 1 << 14 );
		uid = -uid;
	} else {
		uid = -uid;
	}
	return ( uid );
}

owner_t get_path_owner( std::string const& path_ ) {
	owner_t owner;

	HANDLE hFile(
		::CreateFile(
			path_.c_str(),
			GENERIC_READ,
			FILE_SHARE_READ,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
			nullptr
		)
	);
	if ( hFile == INVALID_HANDLE_VALUE ) {
		return ( owner );
	}
	PSID pSidOwner( nullptr );
	PSID pSidGroup( nullptr );
	PSECURITY_DESCRIPTOR pSD( nullptr );
	if (
		::GetSecurityInfo(
			hFile,
			SE_FILE_OBJECT,
			OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION,
			&pSidOwner,
			&pSidGroup,
			nullptr,
			nullptr,
			&pSD
		) == ERROR_SUCCESS
	) {
		owner.first = recode_sid( pSidOwner );
		owner.second = recode_sid( pSidGroup );
	}
	::LocalFree( pSD );
	::CloseHandle( hFile );
	return ( owner );
}

}
