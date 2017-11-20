rem build stdshit
call Lean_3.bat
call egcc.bat
gcc -x c++-header bin\stdshit.h %CCFLAGS2% -o bin\stdshit.h.gch
pushd obj
gcc %CCFLAGS2% -c -g -I..\bin ..\src\*.cc memalloc.cpp ..\src\windows\*.cpp ..\src\windows\utf8-api\*.cpp
gcc %CCFLAGS2% -c -g -I..\bin -DNWIDE2=0 ..\src\xstrfmt.cpp -o xstrfmtA.o
gcc %CCFLAGS2% -c -g -I..\bin -DNWIDE2=1 ..\src\xstrfmt.cpp -o xstrfmtW.o

popd

rem install stdshit
move /Y bin\stdshit.h.gch %prefix%\include\stdshit.h.gch
copy /Y bin\stdshit.h %prefix%\include
copy /Y bin\stdshit.h %PROGRAMS%\local\include
ar rcs %PROGRAMS%\local\lib32\libstdshit.a obj\*.o
