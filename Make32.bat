@setlocal
@call egcc.bat
@if not exist build32 mkdir build32
@pushd build32
@set CFLAGS=%CFLAGS2%
@set CXXFLAGS=%CCFLAGS2%
@%CMAKE% ..
@mingw32-make
@popd

rem install stdshit
copy /Y build32\CMakeFiles\stdshit.dir\cmake_pch.hxx.gch %prefix%\include\stdshit.h.gch
copy /Y bin\stdshit.h %prefix%\include
copy /Y bin\stdshit.h %PROGRAMS%\local\include
copy /Y bin\libstdshit.a %PROGRAMS%\local\lib32
