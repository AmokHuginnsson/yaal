param (
	[Parameter(Mandatory=$True)] [string[]]$target,
	[Parameter(Mandatory=$False)] [string]$prefix = "$pwd/build/windows",
	[Parameter(Mandatory=$False)] [string]$EXTRA_FLAGS,
	[Parameter(Mandatory=$False)] [string]$BUILD_ID,
	[Parameter(Mandatory=$False)] [switch]$auto_setup
)

function make_absolute( [string]$path ) {
	if ( -Not( [System.IO.Path]::IsPathRooted( $path ) ) ) {
		$path = [IO.Path]::GetFullPath( [IO.Path]::Combine( ( ($pwd).Path ), ( $path ) ) )
	}
	return $path.Replace( "\", "/" )
}

function purge {
	Write-Host -NoNewline "Purging... "
	Remove-Item "build" -Recurse -ErrorAction Ignore
	Write-Host "done."
}

function build( [string]$config, [string]$extraFlags ) {
	New-Item -ItemType Directory -Force -Path "build/$config" > $null
	Push-Location "build/$config"
	cscript ../../configure.js PROJECT_ROOT=../../ BUILD_TYPE=$config $extraFlags
	cmake --build . --config $config
	Pop-Location
}

function install( [string]$config ) {
	Push-Location "build/$config"
	cmake --build . --target install --config $config
	Pop-Location
}

function debug( [string]$extraFlags = $EXTRA_FLAGS ) {
	build "debug" $extraFlags
}

function release( [string]$extraFlags = $EXTRA_FLAGS ) {
	build "release" $extraFlags
}

function install-debug {
	debug
	install "debug"
}

function install-release {
	release
	install "release"
}

function package {
	debug "BUILD_PACKAGE=1"
	release "BUILD_PACKAGE=1"
	$packagePath = make_absolute( "build/msi" )
	Push-Location "build/release"
	cpack -B $packagePath
	Pop-Location
}

function bundle {
	package
	$version = ""
	Select-String `
		-ErrorAction Ignore `
		-Path "Makefile.mk.in" `
		-Pattern "VERSION\s*=\s*(\d+)" `
	| ForEach-Object {
		if ( $version -ne "" ) {
			$version += "."
		}
		$version += "$($_.Matches.groups[1])"
	}
	$bundlePath = "build/yaal-deploy/windows/$version"
	Remove-Item "build/yaal-deploy" -Recurse -ErrorAction Ignore
	New-Item -ItemType Directory -Force -Path "$bundlePath" > $null
	$sys = "win32"
	$tag = $version
	if ( $BUILD_ID -ne "" ) {
		$tag += "-$BUILD_ID"
	}
	Move-Item `
		-Path build/msi/yaal-$version-$sys.msi `
		-Destination "$bundlePath/yaal-$tag-$sys.msi" `
		-Force
	Compress-Archive -Path build/yaal-deploy -DestinationPath build/yaal-deploy.zip -Force
}

function auto_setup( $parameters ) {
	if ( $parameters.ContainsKey( "EXTRA_FLAGS" ) ) {
		throw "You cannot specify EXTRA_FLAGS in auto_setup mode!"
	}
	New-Item -ItemType Directory -Force -Path "build/cache" > $null
	$out = "build/cache/windows-dev.zip"
	if ( -Not( Test-Path( $out ) ) ) {
			$EXTRA_FLAGS="EXTRA_FLAGS=YAAL_AUTO_SANITY"
			$uri = "https://codestation.org/download/windows-dev.zip"
			Invoke-WebRequest -Uri $uri -OutFile $out
	}
	if ( -Not( Test-Path( "$prefix/bin/pcre.dll" ) ) ) {
		Expand-Archive -LiteralPath $out -DestinationPath "build/cache" -Force
		$extract = "build/cache/windows/"
		New-Item -ItemType Directory -Force -Path "$prefix" > $null
		Copy-Item -Path "$extract/*" -Destination "$prefix/" -Recurse -Force
		Remove-Item -Path $extract -Recurse -Force
	}
}

foreach ( $t in $target ) {
	if (
		( $t -ne "debug" ) -and
		( $t -ne "release" ) -and
		( $t -ne "install-debug" ) -and
		( $t -ne "install-release" ) -and
		( $t -ne "purge" ) -and
		( $t -ne "package" ) -and
		( $t -ne "bundle" )
	) {
		Write-Error "Unknown target: ``$t``"
		exit 1
	}
}

try {
	$origEnvPath=$env:Path
	$stackSize = ( Get-Location -Stack ).Count
	Push-Location $PSScriptRoot
	if ( Test-Path( "local.js" ) ) {
		Select-String -ErrorAction Ignore -Path "local.js" -Pattern "PREFIX\s*=\s*[`"]([^`"]+)[`"]" | ForEach-Object {
			$prefix = make_absolute( "$($_.Matches.groups[1])" )
		}
	} elseif ( $auto_setup ) {
		$prefix = make_absolute( $prefix )
		$local_js = (
			"PREFIX = `"$prefix`";`n" +
			"SYSCONFDIR = `"$prefix/etc`";`n" +
			"DATADIR = `"$prefix/share`";`n" +
			"LOCALSTATEDIR = `"$prefix/var`";`n" +
			"EXTRA_INCLUDE_PATH = `"$prefix/include`";`n" +
			"EXTRA_LIBRARY_PATH = `"$prefix/lib`";`n" +
			"VERBOSE = 1;`n" +
			"FAST = 1;`n"
		)
		$Utf8NoBomEncoding = New-Object System.Text.UTF8Encoding $False
		[System.IO.File]::WriteAllText( "$pwd/local.js", $local_js, $Utf8NoBomEncoding )
	}
	if ( $auto_setup ) {
		auto_setup $PSBoundParameters
	}
	$env:Path = ( $env:Path.Split( ';')  | Where-Object { -Not( $_.ToLower().Contains( "cygwin" ) ) } ) -join ';'
	$env:Path = "$prefix\bin;$env:Path"
	$env:OPENSSL_CONF = "$prefix/bin/openssl.cfg"
	foreach ( $t in $target ) {
		&$t
	}
} catch {
	Write-Error "$_"
	$LASTEXITCODE = 1
} finally {
	while ( ( Get-Location -Stack ).Count -gt $stackSize ) {
		Pop-Location
	}
	$env:Path=$origEnvPath
	exit $LASTEXITCODE
}

