REM Known failed to build with mingw32-7.2.0-i686-win32-dwarf,
REM complain undefined reference to 'IID_IWbemPath' and
REM 'CLSID_WbemDefPath'.

@echo off
pushd %~dp0\..
if not exist build md build || goto err
gcc -O0 -o "%cd%\build\gcc_c89.exe" ^
	"%cd%\src\main.c" ^
	"%cd%\..\..\impl\_Clevo.c" ^
	"%cd%\..\..\impl\_WbemErr.c" ^
	"%cd%\..\..\impl\Clevo.c" ^
	"%cd%\..\..\impl\Kbled.c" ^
	-I"%cd%\..\..\client" ^
	-loleaut32 -lwbemuuid ^
	-Wall -Wextra -std=c89 -pedantic ^
	-Wstrict-prototypes ^
	-Wold-style-definition -lole32 || goto err
popd
exit/b

:err
popd
exit/b 1
