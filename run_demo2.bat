@echo off
echo Compiling and running FastGhostMouse Demo 2...
call mvn -q clean install -DskipTests
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

java --enable-native-access=ALL-UNNAMED -cp "target\fastghostmouse-0.1.0-jar-with-dependencies.jar" fastghostmouse.FastGhostMouseDemo2
pause
