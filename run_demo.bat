@echo off
:: Enable ANSI colors (Windows 10+)
for /f "tokens=2 delims=: " %%i in ('reg query HKCU\Console /v VirtualTerminalLevel 2^>nul') do set vt=%%i
if not defined vt reg add HKCU\Console /v VirtualTerminalLevel /t REG_DWORD /d 1 /f >nul

echo [\x1b[93m*\x1b[0m] Building Main Project...
call mvn clean package -DskipTests
if %ERRORLEVEL% NEQ 0 ( pause & exit /b )

echo [\x1b[92m>\x1b[0m] Running Hero Demo...
cd examples\Demo
call mvn compile exec:java -Dexec.mainClass=fastghostmouse.demo.Demo
cd ..\..

pause
