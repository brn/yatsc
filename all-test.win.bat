@echo off
set VisualStudioVersion=12.0
set config=/p:Configuration=Debug
set dir=Debug
set args=""
set last_test=""

IF "%1" == "Rebuild" set args=/t:Rebuild
IF "%1" == "Release" (
  set config=/p:Configuration=%1
  set dir=Release
)
IF "%2" == "Rebuild" set args=/t:Rebuild
IF "%2" == "Release" (
  set config=/p:Configuration=%1
  set dir=Release
)

MSBuild.exe test.sln %args% /m /p:Platform=Win32 %config% /p:TargetFrameworkVersion=v4.5.1 /p:PlatformToolset=v120 /toolsversion:12.0
call :RUN AlignedHeapAllocatorTest.exe
call :RUN ChunkHeaderTest.exe
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