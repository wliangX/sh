@echo off
@pushd %~sdp0
title Copy4Debug
SETLOCAL ENABLEDELAYEDEXPANSION

:doShowProcessList
echo.
echo ******************** show process list ***********************************
set i=0
for /f "eol= tokens=1" %%s in (BuildProcessList.txt) do (
	set lnStr=%%s
	if not "!lnStr:~0,1!"==";" (
		set /a i+=1
		echo !i!	%%s
	)	
)
if %i%==0 (
	goto doEnd
)

:doUserSelect
echo.
echo ******************** user select *****************************************
echo please select the process num you want to debug
set /p process_num=
set j=0
for /f "eol= tokens=1" %%s in (BuildProcessList.txt) do (
	set process_name=%%s
	if not "!process_name:~0,1!"==";" (
		set /a j+=1
		if !j!==!process_num! (
			echo !process_name!
			goto doUserSelectConfirm
		)
	)
	set process_name=""
)
goto doUserSelect

:doUserSelectConfirm
echo.
echo ******************** user select confirm *********************************
echo do you want to start copy debug [y/n]
set /p user_confirm=
if not "%user_confirm%"=="y" (
	goto doUserSelect
)

:doCopy4Debug
echo.
echo ******************** start copy debug ************************************
set bld_type=Debug
set icc_path=..\..\..
set bld_path=%icc_path%\icc-server\Build\%bld_type%
set cfg_path=%icc_path%\icc-server\Tools\ConfigFiles
set win_path=%icc_path%\icc-server\ThirdParty\Windows

::copy
for /f "delims=. tokens=1-4" %%a in ("%process_name%") do (
	::init var
	set type=%%c
	set subtype=%%d

	::copy common depend files
	del		/q	                                          				"!bld_path!\*.dll"
	xcopy	/y "!win_path!\activemq\dll\%bld_type%\*.dll" 				"!bld_path!\"
	xcopy	/y "!win_path!\postgres\dll\%bld_type%\*.dll"				"!bld_path!\"
	xcopy	/y "!win_path!\redis\dll\%bld_type%\*.dll" 					"!bld_path!\"
	
	::copy build files
	del		/q															"!bld_path!\Plugins\*.*"
	xcopy	/y "!bld_path!\Plugins\!type!\!subtype!\*.*"				"!bld_path!\Plugins\"
	xcopy	/y "!bld_path!\Plugins\Public\Message.*"					"!bld_path!\Plugins\"
	
	::sth special
	if "!subtype!"=="CTI" (
		xcopy	/y "!win_path!\avaya\TSAPI\dll\%bld_type%\*.dll" 		"!bld_path!\"
		move	/y "!bld_path!\Plugins\TSAPIClient.*"					"!bld_path!\Components\"
	) else (
		del		/q "!bld_path!\Components\TSAPIClient.*"												
	)
	
	::build config.config
	del		/q	                                          				"!bld_path!\Config.Config"
	echo [ICC]>%bld_path%\Config.Config
	echo ConfigPath=..\..\Tools\ConfigFiles\!type!\Config.xml>>%bld_path%\Config.Config
	echo StaticConfigPath=..\..\Tools\ConfigFiles\!type!\StaticConfig.xml>>%bld_path%\Config.Config
	echo SqlConfigPath=..\..\Tools\ConfigFiles\Sql\sql.xml>>%bld_path%\Config.Config
)

echo ******************** finish **********************************************

:doEnd
pause
