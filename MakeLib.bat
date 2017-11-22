rem build stdshit
call Lean_3.bat
call egcc.bat
gcc -x c++-header bin\stdshit.h %CCFLAGS2% -o bin\stdshit.h.gch
pushd obj
set SRC=..\src\math.cpp ..\src\memalloc.cpp ..\src\string.cpp
gcc %CCFLAGS2% -c -g -I..\bin %SRC% ..\src\*.cc ..\src\windows\*.cpp ..\src\windows\utf8-api\*.cpp
gcc %CCFLAGS2% -c -g -I..\bin -DNWIDE2=0 ..\src\xstrfmt.cpp -o xstrfmtA.o
gcc %CCFLAGS2% -c -g -I..\bin -DNWIDE2=1 ..\src\xstrfmt.cpp -o xstrfmtW.o
gcc %CCFLAGS2% -c -g -I..\bin -DNWIDE2=0 ..\src\string8W.cxx -o stringA.o
gcc %CCFLAGS2% -c -g -I..\bin -DNWIDE2=1 ..\src\string8W.cxx -o stringW.o
gcc %CCFLAGS2% -c -g -I..\bin -DNWIDE2=0 ..\src\windows\windows8W.cxx -o windowsA.o
gcc %CCFLAGS2% -c -g -I..\bin -DNWIDE2=1 ..\src\windows\windows8W.cxx -o windowsW.o

popd

rem install stdshit
move /Y bin\stdshit.h.gch %prefix%\include\stdshit.h.gch
copy /Y bin\stdshit.h %prefix%\include
copy /Y bin\stdshit.h %PROGRAMS%\local\include
ar rcs %PROGRAMS%\local\lib32\libstdshit.a obj\*.o
