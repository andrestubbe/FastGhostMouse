@echo off
chcp 65001 >nul


echo ðŸš€ Running Demo...
cd examples\Demo
set MAVEN_OPTS=--enable-native-access=ALL-UNNAMED --sun-misc-unsafe-memory-access=allow
call mvn -q compile exec:java -Dexec.mainClass=fastghostmouse.demo.Demo
cd ..\..
pause
