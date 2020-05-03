@call cmake_gcc

rem install stdshit
@setlocal
@call egcc.bat
copy /Y build32\CMakeFiles\stdshit.dir\cmake_pch.hxx.gch %prefix%\include\stdshit.h.gch
copy /Y bin\stdshit.h %prefix%\include
copy /Y bin\stdshit.h %PROGRAMS%\local\include
copy /Y bin\libstdshit.a %PROGRAMS%\local\lib32
call install.bat
