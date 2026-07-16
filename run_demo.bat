@echo off
chcp 65001 >nul
cd /d "%~dp0"
setlocal

echo ⚡ Building Project...
call mvn clean package -DskipTests -q
if %ERRORLEVEL% NEQ 0 ( echo ❌ Benchmark failed. & pause & exit /b %ERRORLEVEL% )

echo 🚀 Running Demo...
cd examples\Demo
set MAVEN_OPTS=--enable-native-access=ALL-UNNAMED --sun-misc-unsafe-memory-access=allow
call mvn -q compile exec:java -Dexec.mainClass=fastghostmouse.demo.Demo -q
if %ERRORLEVEL% NEQ 0 ( echo ❌ Benchmark failed. & pause & exit /b %ERRORLEVEL% )

cd ..
endlocal
