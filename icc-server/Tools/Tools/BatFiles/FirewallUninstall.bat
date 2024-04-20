@echo off

set exe_file1=%~sdp0Client\ICC2.0.exe
if exist "%exe_file1%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Client.in dir=in && (@echo ok[1]) || (goto :xp1)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Client.out dir=out && (goto :next1) || (goto :xp1)

    :xp1
    netsh firewall delete allowedprogram program="%exe_file1%"
)
:next1

set exe_file2=%~sdp0Server\SDG\CommandCenter.ICC.Server.ServiceDog.exe
if exist "%exe_file2%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.ServiceDog.in dir=in && (@echo ok[2]) || (goto :xp2)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.ServiceDog.out dir=out && (goto :next2) || (goto :xp2)

    :xp2
    netsh firewall delete allowedprogram program="%exe_file2%"
)
:next2

set exe_file3=%~sdp0Server\SDG\CommandCenter.ICC.Server.Monitor.exe
if exist "%exe_file3%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.Monitor.in dir=in && (@echo ok[3]) || (goto :xp3)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.Monitor.out dir=out && (goto :next3) || (goto :xp3)

    :xp3
    netsh firewall delete allowedprogram program="%exe_file3%"
)
:next3

set exe_file4=%~sdp0Gateway\SDG\CommandCenter.ICC.Gateway.ServiceDog.exe
if exist "%exe_file4%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.ServiceDog.in dir=in && (@echo ok[4]) || (goto :xp4)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.ServiceDog.out dir=out && (goto :next4) || (goto :xp4)

    :xp4
    netsh firewall delete allowedprogram program="%exe_file4%"
)
:next4

set exe_file5=%~sdp0Gateway\SDG\CommandCenter.ICC.Gateway.Monitor.exe
if exist "%exe_file5%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.Monitor.in dir=in && (@echo ok[5]) || (goto :xp5)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.Monitor.out dir=out && (goto :next5) || (goto :xp5)

    :xp5
    netsh firewall delete allowedprogram program="%exe_file5%"
)
:next5

set exe_file6=%~sdp0Server\CommandCenter.ICC.Server.Alarm\CommandCenter.ICC.Server.Alarm.exe
if exist "%exe_file6%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.Alarm.in dir=in && (@echo ok[6]) || (goto :xp6)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.Alarm.out dir=out && (goto :next6) || (goto :xp6)

    :xp6
    netsh firewall delete allowedprogram program="%exe_file6%"
)
:next6

set exe_file7=%~sdp0Server\CommandCenter.ICC.Server.BaseData\CommandCenter.ICC.Server.BaseData.exe
if exist "%exe_file7%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.BaseData.in dir=in && (@echo ok[7]) || (goto :xp7)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.BaseData.out dir=out && (goto :next7) || (goto :xp7)

    :xp7
    netsh firewall delete allowedprogram program="%exe_file7%"
)
:next7

set exe_file8=%~sdp0Server\CommandCenter.ICC.Server.CTI\CommandCenter.ICC.Server.CTI.exe
if exist "%exe_file8%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.CTI.in dir=in && (@echo ok[8]) || (goto :xp8)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.CTI.out dir=out && (goto :next8) || (goto :xp8)

    :xp8
    netsh firewall delete allowedprogram program="%exe_file8%"
)
:next8

set exe_file9=%~sdp0Server\CommandCenter.ICC.Server.DBAgent\CommandCenter.ICC.Server.DBAgent.exe
if exist "%exe_file9%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.DBAgent.in dir=in && (@echo ok[9]) || (goto :xp9)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Server.DBAgent.out dir=out && (goto :next9) || (goto :xp9)

    :xp9
    netsh firewall delete allowedprogram program="%exe_file9%"
)
:next9

set exe_file10=%~sdp0Gateway\CommandCenter.ICC.Gateway.Fast110\CommandCenter.ICC.Gateway.Fast110.exe
if exist "%exe_file10%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.Fast110.in dir=in && (@echo ok[10]) || (goto :xp10)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.Fast110.out dir=out && (goto :next10) || (goto :xp10)

    :xp10
    netsh firewall delete allowedprogram program="%exe_file10%"
)
:next10

set exe_file11=%~sdp0Gateway\CommandCenter.ICC.Gateway.GPS\CommandCenter.ICC.Gateway.GPS.exe
if exist "%exe_file11%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.GPS.in dir=in && (@echo ok[11]) || (goto :xp11)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.GPS.out dir=out && (goto :next11) || (goto :xp11)

    :xp11
    netsh firewall delete allowedprogram program="%exe_file11%"
)
:next11

set exe_file12=%~sdp0Gateway\CommandCenter.ICC.Gateway.IFDS\CommandCenter.ICC.Gateway.IFDS.exe
if exist "%exe_file12%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.IFDS.in dir=in && (@echo ok[12]) || (goto :xp12)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.IFDS.out dir=out && (goto :next12) || (goto :xp12)

    :xp12
    netsh firewall delete allowedprogram program="%exe_file12%"
)
:next12

set exe_file13=%~sdp0Gateway\CommandCenter.ICC.Gateway.LSP\CommandCenter.ICC.Gateway.LSP.exe
if exist "%exe_file13%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.LSP.in dir=in && (@echo ok[13]) || (goto :xp13)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.LSP.out dir=out && (goto :next13) || (goto :xp13)

    :xp13
    netsh firewall delete allowedprogram program="%exe_file13%"
)
:next13

set exe_file14=%~sdp0Gateway\CommandCenter.ICC.Gateway.MPA\CommandCenter.ICC.Gateway.MPA.exe
if exist "%exe_file14%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.MPA.in dir=in && (@echo ok[14]) || (goto :xp14)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.MPA.out dir=out && (goto :next14) || (goto :xp14)

    :xp14
    netsh firewall delete allowedprogram program="%exe_file14%"
)
:next14

set exe_file15=%~sdp0Gateway\CommandCenter.ICC.Gateway.MRPS\CommandCenter.ICC.Gateway.MRPS.exe
if exist "%exe_file15%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.MRPS.in dir=in && (@echo ok[15]) || (goto :xp15)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.MRPS.out dir=out && (goto :next15) || (goto :xp15)

    :xp15
    netsh firewall delete allowedprogram program="%exe_file15%"
)
:next15

set exe_file16=%~sdp0Gateway\CommandCenter.ICC.Gateway.SMP\CommandCenter.ICC.Gateway.SMP.exe
if exist "%exe_file16%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.SMP.in dir=in && (@echo ok[16]) || (goto :xp16)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.SMP.out dir=out && (goto :next16) || (goto :xp16)

    :xp16
    netsh firewall delete allowedprogram program="%exe_file16%"
)
:next16

set exe_file17=%~sdp0Gateway\CommandCenter.ICC.Gateway.SmsLJ\CommandCenter.ICC.Gateway.SmsLJ.exe
if exist "%exe_file17%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.SmsLJ.in dir=in && (@echo ok[17]) || (goto :xp17)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.SmsLJ.out dir=out && (goto :next17) || (goto :xp17)

    :xp17
    netsh firewall delete allowedprogram program="%exe_file17%"
)
:next17

set exe_file18=%~sdp0Gateway\CommandCenter.ICC.Gateway.SMT\CommandCenter.ICC.Gateway.SMT.exe
if exist "%exe_file18%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.SMT.in dir=in && (@echo ok[18]) || (goto :xp18)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.SMT.out dir=out && (goto :next18) || (goto :xp18)

    :xp18
    netsh firewall delete allowedprogram program="%exe_file18%"
)
:next18

set exe_file19=%~sdp0Gateway\CommandCenter.ICC.Gateway.ThirdAlarm\CommandCenter.ICC.Gateway.ThirdAlarm.exe
if exist "%exe_file19%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.ThirdAlarm.in dir=in && (@echo ok[19]) || (goto :xp19)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.ThirdAlarm.out dir=out && (goto :next19) || (goto :xp19)

    :xp19
    netsh firewall delete allowedprogram program="%exe_file19%"
)
:next19

set exe_file20=%~sdp0Gateway\CommandCenter.ICC.Gateway.VCS\CommandCenter.ICC.Gateway.VCS.exe
if exist "%exe_file20%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.VCS.in dir=in && (@echo ok[20]) || (goto :xp20)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.VCS.out dir=out && (goto :next20) || (goto :xp20)

    :xp20
    netsh firewall delete allowedprogram program="%exe_file20%"
)
:next20

set exe_file21=%~sdp0Gateway\CommandCenter.ICC.Gateway.Web\CommandCenter.ICC.Gateway.Web.exe
if exist "%exe_file21%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.Web.in dir=in && (@echo ok[21]) || (goto :xp21)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.Web.out dir=out && (goto :next21) || (goto :xp21)

    :xp21
    netsh firewall delete allowedprogram program="%exe_file21%"
)
:next21

set exe_file22=%~sdp0Gateway\CommandCenter.ICC.Gateway.WeChatLJ\CommandCenter.ICC.Gateway.WeChatLJ.exe
if exist "%exe_file22%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.WeChatLJ.in dir=in && (@echo ok[22]) || (goto :xp22)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.WeChatLJ.out dir=out && (goto :next22) || (goto :xp22)

    :xp22
    netsh firewall delete allowedprogram program="%exe_file22%"
)
:next22

set exe_file23=%~sdp0Gateway\CommandCenter.ICC.Gateway.WeChatST\CommandCenter.ICC.Gateway.WeChatST.exe
if exist "%exe_file23%" (
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.WeChatST.in dir=in && (@echo ok[23]) || (goto :xp23)
    netsh advfirewall firewall delete rule name=CommandCenter.ICC.Gateway.WeChatST.out dir=out && (goto :next23) || (goto :xp23)

    :xp23
    netsh firewall delete allowedprogram program="%exe_file23%"
)
:next23
