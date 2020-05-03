@call cmake_gcc x64

rem install stdshit
@setlocal
@call egcc.bat x64
copy /Y build64\CMakeFiles\stdshit.dir\cmake_pch.hxx.gch %prefix%\include\stdshit.h.gch
copy /Y bin\stdshit.h %prefix%\include
copy /Y bin\stdshit.h %PROGRAMS%\local\include
copy /Y bin\libstdshit.a %PROGRAMS%\local\lib64
