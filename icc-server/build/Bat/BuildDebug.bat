@echo off
@pushd %~sdp0
title Build ICCServer

if "%VS120COMNTOOLS%"=="" goto doNotFindVS120

echo compile and package at %date% %time%
echo register environment var ......
call "%VS120COMNTOOLS%VsDevCmd.bat"
echo waiting for build ICCServer.sln ......
MSBuild "..\ICCServer.sln" /t:Build /p:Configuration=Debug /nologo /m /clp:ErrorsOnly /flp:Summary;Verbosity=minimal;LogFile=BuildDebug.log
echo finish at %date% %time%
goto end

:doNotFindVS120
echo not find VS2013
goto end

:end
pause