@echo off
@pushd %~sdp0
SETLOCAL ENABLEDELAYEDEXPANSION

set ins_bat_file=FirewallInstall.bat
set uni_bat_file=FirewallUninstall.bat

echo @echo off>!ins_bat_file!
echo @echo off>!uni_bat_file!

set p=%%
set i=0
for /f "tokens=1" %%s in (BuildProcessList.txt) do (
	for /f "delims=. tokens=1-3,*" %%a in ("%%s") do (

		set type=%%c
		set subtype=%%d
		
		set dir_path=!type!\%%a.%%b.%%c.%%d\
		if "!subtype!"=="" (
			set exe_name=%%a.%%b.%%c
		) else (
			set exe_name=%%a.%%b.%%c.%%d
		)
		
		set /a i+=1
		set exe_file=exe_file!i!
		set rule_in=!exe_name!.in
		set rule_out=!exe_name!.out
		set rule_inout=!exe_name!
		
		if "!type!"=="Client" (
			set dir_path=!type!\
			set exe_name=ICC2.0
		)
		if "!subtype!"=="ServiceDog" (
			set dir_path=!type!\SDG\
		)
		if "!subtype!"=="Monitor" (
			set dir_path=!type!\SDG\
		)
		
		echo.>>!ins_bat_file!
		echo set !exe_file!=!p!~sdp0!dir_path!!exe_name!.exe>>!ins_bat_file!
		echo if exist "%%!exe_file!%%" ^(>>!ins_bat_file!
		echo     netsh advfirewall firewall add rule name=!rule_in! dir=in action=allow program="%%!exe_file!%%" enable=yes edge=yes ^&^& ^(@echo ok[!i!]^) ^|^| ^(goto :xp!i!^)>>!ins_bat_file!
		echo     netsh advfirewall firewall add rule name=!rule_out! dir=out action=allow program="%%!exe_file!%%" enable=yes ^&^& ^(goto :next!i!^) ^|^| ^(goto :xp!i!^)>>!ins_bat_file!
		echo.>>!ins_bat_file!
		echo     :xp!i!>>!ins_bat_file!
		echo     netsh firewall add allowedprogram program="%%!exe_file!%%" name=!rule_inout! mode=enable scope=all profile=all>>!ins_bat_file!
		echo ^)>>!ins_bat_file!
		echo :next!i!>>!ins_bat_file!

		echo.>>!uni_bat_file!
		echo set !exe_file!=!p!~sdp0!dir_path!!exe_name!.exe>>!uni_bat_file!
		echo if exist "%%!exe_file!%%" ^(>>!uni_bat_file!
		echo     netsh advfirewall firewall delete rule name=!rule_in! dir=in ^&^& ^(@echo ok[!i!]^) ^|^| ^(goto :xp!i!^)>>!uni_bat_file!
		echo     netsh advfirewall firewall delete rule name=!rule_out! dir=out ^&^& ^(goto :next!i!^) ^|^| ^(goto :xp!i!^)>>!uni_bat_file!
		echo.>>!uni_bat_file!
		echo     :xp!i!>>!uni_bat_file!
		echo     netsh firewall delete allowedprogram program="%%!exe_file!%%">>!uni_bat_file!
		echo ^)>>!uni_bat_file!
		echo :next!i!>>!uni_bat_file!
	)
)

:doEnd