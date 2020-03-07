param (
	[Parameter(Mandatory=$True)] [string]$target,
	[Parameter(Mandatory=$False)] [string]$EXTRA_FLAGS,
	[Parameter(Mandatory=$False)] [switch]$auto_setup
)

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
	Push-Location "build/release"
	$packagePath = Resolve-Path "../../build/msi"
	cpack -B $packagePath
}

function auto_setup {
	if ( $PSBoundParameters.ContainsKey( "EXTRA_FLAGS" ) ) {
		throw "You cannot specify $EXTRA_FLAGS in $auto_setup mode!"
	}
	New-Item -ItemType Directory -Force -Path "build/cache" > $null
	$out = "build/cache/windows-dev.zip"
	if ( -Not( Test-Path( $out ) ) ) {
			$EXTRA_FLAGS="EXTRA_FLAGS=YAAL_AUTO_SANITY"
			$uri = "https://codestation.org/download/windows-dev.zip"
			Invoke-WebRequest -Uri $uri -OutFile $out
	}
	if ( -Not( Test-Path( "build/windows/bin/pcre.dll" ) ) ) {
		Expand-Archive -LiteralPath $out -DestinationPath "build/"
	}
	if ( -Not( Test-Path( "local.js" ) ) ) {
		$winPath = "$pwd/build/windows".Replace( "\", "/" )
		$local_js = (
			"PREFIX = `"$winPath`";`n" +
			"SYSCONFDIR = `"$winPath/etc`";`n" +
			"DATADIR = `"$winPath/share`";`n" +
			"LOCALSTATEDIR = `"$winPath/var`";`n" +
			"EXTRA_INCLUDE_PATH = `"$winPath/include`";`n" +
			"EXTRA_LIBRARY_PATH = `"$winPath/lib`";`n" +
			"VERBOSE = 1;`n" +
			"FAST = 1;`n"
		)
		$Utf8NoBomEncoding = New-Object System.Text.UTF8Encoding $False
		[System.IO.File]::WriteAllText( "$pwd/local.js", $local_js, $Utf8NoBomEncoding )
	}
}

if (
	( $target -ne "debug" ) -and
	( $target -ne "release" ) -and
	( $target -ne "install-debug" ) -and
	( $target -ne "install-release" ) -and
	( $target -ne "purge" ) -and
	( $target -ne "package" ) -and
	( $target -ne "deploy" )
) {
	Write-Error "Unknown target: ``$target``"
	exit 1
}

if ( $auto_setup ) {
	auto_setup
}

$origEnvPath=$env:Path

Select-String -ErrorAction Ignore -Path "local.js" -Pattern "PREFIX\s=\s[`"]([^`"]+)[`"]" | ForEach-Object {
	$prefix="$($_.Matches.groups[1])"
	$env:Path="$prefix\bin;$env:Path"
	$env:OPENSSL_CONF="$prefix/bin/openssl.cfg"
}

try {
	&$target
} finally {
	$env:Path=$origEnvPath
	Pop-Location
}

