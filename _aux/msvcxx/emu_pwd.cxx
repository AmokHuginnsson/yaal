#include <accctrl.h>
#include <aclapi.h>

#include "emu_pwd.hxx"

namespace msvcxx {

owner_t get_path_owner( std::string const& path_ ) {
	owner_t owner;

	HANDLE hFile( ::CreateFile(
		path_.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
		NULL ) );
	if ( hFile != INVALID_HANDLE_VALUE ) {
		PSID pSidOwner( NULL );
		PSID pSidGroup( NULL );
		PSECURITY_DESCRIPTOR pSD( NULL );
		if ( ::GetSecurityInfo(
			hFile,
			SE_FILE_OBJECT,
			OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION,
			&pSidOwner,
			&pSidGroup,
			NULL,
			NULL,
			&pSD ) == ERROR_SUCCESS ) {
			owner.first = ( *GetSidSubAuthority( pSidOwner, *GetSidSubAuthorityCount( pSidOwner ) - 1 ) );
			owner.second = ( *GetSidSubAuthority( pSidGroup, *GetSidSubAuthorityCount( pSidGroup ) - 1 ) );
		}
		::LocalFree( pSD );
		::CloseHandle( hFile );
	}
	return ( owner );
}

}
