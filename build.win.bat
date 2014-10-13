@echo off
set VisualStudioVersion=12.0
set config=""
set args=""
IF "%1" == "Rebuild" set args=/t:Rebuild
IF "%1" == "Release" set config=/p:Configuration=%1
IF "%2" == "Rebuild" set args=/t:Rebuild
IF "%2" == "Release" set config=/p:Configuration=%2

MSBuild.exe celero.sln %args% /m /p:Platform=x64 %config% /p:TargetFrameworkVersion=v4.5.1 /p:PlatformToolset=v120 /toolsversion:12.0 &&^
MSBuild.exe perf-test.sln %args% /m /p:Platform=x64 %config% /p:TargetFrameworkVersion=v4.5.1 /p:PlatformToolset=v120 /toolsversion:12.0 &&^
MSBuild.exe test.sln %args% /m /p:Platform=x64 %config% /p:TargetFrameworkVersion=v4.5.1 /p:PlatformToolset=v120 /toolsversion:12.0 &&