@echo off
setlocal enabledelayedexpansion

set install_dir=%~sdp0..\\..\\
if not exist "%install_dir%" goto doEnd

set server_dir=%install_dir%Server
if exist "%server_dir%" (
	for /f "usebackq delims=" %%i in (`dir /b /ad "%server_dir%"`) do (
		for /f "delims=. tokens=1-4" %%a in ("%%i") do (
			set type=%%c
			set subtype=%%d
			set process_path=!server_dir!\%%i

			if "%%a.%%b.%%c.%%d"=="CommandCenter.ICC.Server.Alarm" (
				echo [ICC]>!process_path!\Config.Config
				echo ConfigPath=%ProgramData%\CommandCenter\ICC\ConfigFiles\!type!\Config.xml>>!process_path!\Config.Config
				echo StaticConfigPath=..\..\Tools\ConfigFiles\!type!\StaticConfig.xml>>!process_path!\Config.Config
				echo SqlConfigPath=..\..\Tools\ConfigFiles\Sql\sql.xml>>!process_path!\Config.Config			
			)
			
			if "%%a.%%b.%%c.%%d"=="CommandCenter.ICC.Server.CTI" (
				echo [ICC]>!process_path!\Config.Config
				echo ConfigPath=%ProgramData%\CommandCenter\ICC\ConfigFiles\!subtype!\Config.xml>>!process_path!\Config.Config
				echo StaticConfigPath=..\..\Tools\ConfigFiles\!subtype!\StaticConfig.xml>>!process_path!\Config.Config
				echo SqlConfigPath=..\..\Tools\ConfigFiles\Sql\sql.xml>>!process_path!\Config.Config			
			)
		)
	)
)

set gateway_dir=%install_dir%Gateway
if exist "%gateway_dir%" (
	for /f "usebackq delims=" %%i in (`dir /b /ad "%gateway_dir%"`) do (
		for /f "delims=. tokens=1-3" %%a in ("%%i") do (
			set type=%%c
			set process_path=!gateway_dir!\%%i

			if "%%a.%%b."=="CommandCenter.ICC." (
				echo [ICC]>!process_path!\Config.Config
				echo ConfigPath=%ProgramData%\CommandCenter\ICC\ConfigFiles\!type!\Config.xml>>!process_path!\Config.Config
				echo StaticConfigPath=..\..\Tools\ConfigFiles\!type!\StaticConfig.xml>>!process_path!\Config.Config
				echo SqlConfigPath=..\..\Tools\ConfigFiles\Sql\sql.xml>>!process_path!\Config.Config
			)
		)
	)
)

:doEnd