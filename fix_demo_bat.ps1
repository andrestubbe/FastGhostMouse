$bat = @"
@echo off
chcp 65001 >nul

echo ⚡ Building Main Project...
call mvn -q clean install -DskipTests
if %ERRORLEVEL% NEQ 0 ( pause & exit /b )

echo 🚀 Running Demo...
cd examples\Demo
set MAVEN_OPTS=--enable-native-access=ALL-UNNAMED --sun-misc-unsafe-memory-access=allow
call mvn -q compile exec:java -Dexec.mainClass=fastghostmouse.demo.Demo
cd ..\..
pause
"@

$bat = $bat -replace "`n", "`r`n"
$bat = $bat -replace "`r`r`n", "`r`n"

Set-Content -Path 'C:\Users\andre\Documents\2026-05-17-Work-FastJava\FastGhostMouse\run_demo.bat' -Value $bat -Encoding UTF8
