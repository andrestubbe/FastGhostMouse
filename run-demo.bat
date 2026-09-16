@echo off
setlocal
cd /d "%~dp0"

echo [1/3] Building FastGhostMouse...
call mvn clean install -DskipTests -q
if %ERRORLEVEL% NEQ 0 ( echo [ERROR] FastGhostMouse build failed. & pause & exit /b %ERRORLEVEL% )

powershell -NoProfile -Command "Unblock-File -Path '%USERPROFILE%\.fastcore\native\fastghostmouse\*', '%~dp0build\*', '%~dp0src\main\resources\*' -ErrorAction SilentlyContinue" >nul 2>&1

echo [2/3] Compiling Demo...
cd examples\Demo
call mvn compile dependency:build-classpath "-Dmdep.outputFile=cp.txt" "-DincludeScope=runtime" -q
if %ERRORLEVEL% NEQ 0 ( echo [ERROR] Demo build failed. & cd ..\.. & pause & exit /b %ERRORLEVEL% )

echo [3/3] Running FastGhostMouse Demo...
set /p CP=<cp.txt
java --enable-native-access=ALL-UNNAMED --sun-misc-unsafe-memory-access=allow "-Djava.library.path=%~dp0build;%~dp0src\main\resources\native" -cp "target\classes;%CP%" fastghostmouse.demo.Demo

cd ..\..
pause
