@echo off
echo Building Main Project...
call mvn -q clean install -DskipTests
if %ERRORLEVEL% NEQ 0 (
    pause
    exit /b
)

echo Running Demo...
cd examples\Demo
call mvn -q compile exec:java -Dexec.mainClass=fastghostmouse.demo.Demo
cd ..\..

pause
