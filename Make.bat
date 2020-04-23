call egcc.bat

:: build library
gcc process.cc %CCFLAGS2% -o bin\process.o -c
ar rcs %PROGRAMS%\local\lib32\libexshit.a bin\process.o
copy /Y process.h %PROGRAMS%\local\include\dfProcThrd.h 

:: build profiler
gcc profiler.cc proflog.cc %CCFLAGS2% -lexshit -lstdshit -o bin\dfprof.exe -s


