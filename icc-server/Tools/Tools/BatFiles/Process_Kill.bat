@echo off

::查杀指定进程
taskkill /f /im ICC2.0.exe 2>nul 1>nul
ping localhoat -n 6 2>nul 1>nul

::查杀CommandCenter.ICC.Server.X.exe进程
net stop CommandCenter.ICC.Server.ServiceDog 2>nul 1>nul
taskkill /f /im CommandCenter.ICC.Server.* 2>nul 1>nul
ping localhoat -n 6 2>nul 1>nul

::查杀CommandCenter.ICC.Gateway.X.exe进程
net stop CommandCenter.ICC.Gateway.ServiceDog 2>nul 1>nul
taskkill /f /im CommandCenter.ICC.Gateway.* 2>nul 1>nul
ping localhoat -n 6 2>nul 1>nul

