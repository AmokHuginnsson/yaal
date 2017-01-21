@echo OFF

if "%1" == "" (
	echo "Destination directory not specified."
	exit /b 1
)

set DEST_DIR="%1"

md "%DEST_DIR%"
set OLDPWD=%cd%
cd "%DEST_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%

set COUNTRY=XX
set STATE=State
set LOCATION=Location
set ORGARNIZATION=Organization
set ORGARNIZATION_UNIT=Organization Unit
set COMMON_NAME=example.codestation.org
set EMAIL_ADDRESS=admin@example.codestation.org

set SUBJECT=
set SUBJECT=%SUBJECT%/C=%COUNTRY%
set SUBJECT=%SUBJECT%/ST=%STATE%
set SUBJECT=%SUBJECT%/L=%LOCATION%
set SUBJECT=%SUBJECT%/O=%ORGARNIZATION%
set SUBJECT=%SUBJECT%/OU=%ORGARNIZATION_UNIT%
set SUBJECT=%SUBJECT%/CN=%COMMON_NAME%
set SUBJECT=%SUBJECT%/emailAddress=%EMAIL_ADDRESS%/

openssl genpkey -algorithm rsa -pkeyopt rsa_keygen_bits:2048 -out key
if %errorlevel% neq 0 exit /b %errorlevel%
openssl req -config %PREFIX%/bin/openssl.cnf -x509 -new -key key -out pem -days 3652 -subj "%SUBJECT%"
if %errorlevel% neq 0 exit /b %errorlevel%

cd "%OLDPWD%"

