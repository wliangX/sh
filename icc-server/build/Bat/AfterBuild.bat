@echo off
@pushd %~sdp0
SETLOCAL ENABLEDELAYEDEXPANSION

rem check param
set bld_type=%~1
if "%bld_type%"=="Release" goto doCopy
if "%bld_type%"=="Debug"   goto doCopy

:doUserInput
echo please select the build type (1:Debug 2:Release)
set /p user_select=
if "%user_select%"=="1" (
	set bld_type=Debug
	goto doCopy
)
if "%user_select%"=="2" (
	set bld_type=Release
	goto doCopy
)
goto doUserInput

:doCopy

rem init path
set icc_path=..\..
set bld_path=%icc_path%\Build\%bld_type%
set pkg_path=%icc_path%\Build\Package\%bld_type%
set pdb_path=%icc_path%\Build\Pdb\%bld_type%
set tls_path=%icc_path%\Tools
set win_path=%icc_path%\ThirdParty\Windows

rem rd /s /Q %pkg_path%
rem rd /s /Q %pdb_path%

rem copy package
for /f "tokens=1" %%s in (BuildProcessList.txt) do (
	for /f "delims=. tokens=1-4" %%a in ("%%s") do (
		rem init var
		set type=%%c
		set subtype=%%d
		if "!subtype!"=="CTI" (
			set srv_name=%%a.%%b.%%c.%%d
			set pkg_srv_path=!pkg_path!\!type!\%%a.%%b.%%c.CTI
		) else (
			if "!type!"=="Server" (
				set srv_name=%%a.%%b.%%c.Alarm
				set pkg_srv_path=!pkg_path!\!type!\%%a.%%b.%%c.Alarm
			) else (
				set srv_name=%%a.%%b.%%c
				set pkg_srv_path=!pkg_path!\!type!\%%a.%%b.%%c
			)
		)

		xcopy /y "!win_path!\activemq\dll\!bld_type!\*.dll" 					"!pkg_srv_path!\"
		xcopy /y "!win_path!\postgres\dll\!bld_type!\*.dll"						"!pkg_srv_path!\"
		xcopy /y "!win_path!\redis\dll\!bld_type!\*.dll" 						"!pkg_srv_path!\"
		
		copy  /y "!bld_path!\Console.exe"		 								"!pkg_srv_path!\!srv_name!.exe"
		xcopy /y "!bld_path!\Frame.frm" 										"!pkg_srv_path!\"
		xcopy /y "!bld_path!\Manager\*.mng" 									"!pkg_srv_path!\Manager\"
		xcopy /y "!bld_path!\Components\*.com" 									"!pkg_srv_path!\Components\"
		xcopy /y "!bld_path!\Plugins\!type!\!subtype!\*.plg"					"!pkg_srv_path!\Plugins\"		
		copy /y "!bld_path!\Plugins\Public\Message.plg"							"!pkg_srv_path!\Plugins\"

		rem sth special
		if "!subtype!"=="CTI" (
			xcopy /y "!win_path!\avaya\TSAPI\dll\!bld_type!\*.dll" 				"!pkg_srv_path!\"
			copy  /y "!bld_path!\Plugins\!type!\!subtype!\TSAPIClient.com"   	"!pkg_srv_path!\Components\"
		)
		
		rem copy servicedog only Release
		if "!bld_type!"=="Release" (
			copy  /y "!win_path!\servicedog\!type!\DogClient.xml" 								"!pkg_srv_path!\"
			copy  /y "!win_path!\servicedog\!type!\CommandCenter.DogClientWrapper.dll"			"!pkg_srv_path!\"
			copy  /y "!win_path!\servicedog\!type!\CommandCenter.ServiceDog.DogClientPlug.dll" 	"!pkg_srv_path!\"			
			copy  /y "!bld_path!\Plugins\Public\DogClient.plg"									"!pkg_srv_path!\Plugins\" 
			
			rem copy servicedog just once
			if "!subtype!"=="Alarm" (
				xcopy /y "!win_path!\servicedog\!type!\*.*" 							"!pkg_srv_path!\..\SDG\"
			)
			
			rem copy servicedog just once
			if "!subtype!"=="SMP" (
				xcopy /y "!win_path!\servicedog\!type!\*.*" 							"!pkg_srv_path!\..\SDG\"
			)
		)		
	)
	
	rem copy config file
	echo [ICC]>!pkg_srv_path!\Config.Config
	if "!subtype!" NEQ "CTI" (
		echo ConfigPath=..\..\Tools\ConfigFiles\!type!\Config.xml>>!pkg_srv_path!\Config.Config
		echo SqlConfigPath=..\..\Tools\ConfigFiles\Sql\sql.xml>>!pkg_srv_path!\Config.Config
		echo StaticConfigPath=..\..\Tools\ConfigFiles\!type!\StaticConfig.xml>>!pkg_srv_path!\Config.Config
	) else (
		echo ConfigPath=..\..\Tools\ConfigFiles\!subtype!\Config.xml>>!pkg_srv_path!\Config.Config
		echo SqlConfigPath=..\..\Tools\ConfigFiles\Sql\sql.xml>>!pkg_srv_path!\Config.Config
		echo StaticConfigPath=..\..\Tools\ConfigFiles\!subtype!\StaticConfig.xml>>!pkg_srv_path!\Config.Config
	)
)

::copy Tools
xcopy /y /e		"%tls_path%"		 		"%pkg_path%\Tools\"

::copy pdb
xcopy /y /e		"%bld_path%\*.pdb" 			"%pdb_path%\"

:doEnd