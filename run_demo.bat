@echo off
chcp 65001 >nul

:: Enable ANSI (Windows 10+)
for /f "tokens=2 delims=: " %%i in ('reg query HKCU\Console /v VirtualTerminalLevel 2^>nul') do set vt=%%i
if not defined vt reg add HKCU\Console /v VirtualTerminalLevel /t REG_DWORD /d 1 /f >nul

echo ⚡ Building Main Project...
call mvn -q clean package -DskipTests
if %ERRORLEVEL% NEQ 0 ( pause & exit /b )

echo 🚀 Running Hero Demo...
cd examples\Demo
call mvn -q compile exec:java -Dexec.mainClass=fastghostmouse.demo.Demo
cd ..\..

pause
