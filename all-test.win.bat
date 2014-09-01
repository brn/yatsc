@echo off
set VisualStudioVersion=12.0
set config=/p:Configuration=Debug
set dir=Debug
set args=""
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

MSBuild.exe test.sln %args% /m /p:Platform=Win32 %config% /p:TargetFrameworkVersion=v4.5.1 /p:PlatformToolset=v120 /toolsversion:12.0 && "%dir%/SourceStreamTest.exe" && "%dir%/UnicodeIteratorAdapterTest.exe" && "%dir%/ScannerTest.exe" && "%dir%/RegionsTest.exe" && "%dir%/NodeTest.exe" && "%dir%/ExpressionParseTest.exe" && "%dir%/DeclarationParseTest.exe" && "%dir%/StatementParseTest.exe"