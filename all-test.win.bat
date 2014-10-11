@echo off

set config=""
set args=""
set last_test=""
set %dir%=""

IF "%1" == "Release" set dir=./Release
IF "%1" == "Debug" set dir=./Debug
IF "%2" == "Release" set dir=./Release
IF "%2" == "Debug" set dir=./Debug

call ./build.win.bat %*
call :RUN IntrusiveRBTreeTest.exe
call :RUN AlignedHeapAllocatorTest.exe
call :RUN ChunkHeaderTest.exe
call :RUN ArenaTest.exe
call :RUN SourceStreamTest.exe
call :RUN UnicodeIteratorAdapterTest.exe
call :RUN ScannerTest.exe
call :RUN NodeTest.exe
call :RUN ExpressionParseTest.exe
call :RUN DeclarationParseTest.exe
call :RUN StatementParseTest.exe

exit

:RUN
if %ERRORLEVEL% == 0 (set last_test=%1 & "%dir%/%1") else (goto ERROR)
exit /b

:ERROR
echo %last_test% failed with return code %ERRORLEVEL%
exit %ERRORLEVEL%