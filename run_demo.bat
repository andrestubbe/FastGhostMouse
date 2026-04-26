@echo off
echo ========================================
echo FastGhostMouse Demo Runner
echo ========================================

:: Compile native DLL
echo [1/4] Compiling native DLL...
call compile.bat

:: Build with Maven
echo [2/4] Building with Maven...
call .\mvnw.cmd clean package

:: Compile Demo
echo [3/4] Compiling Demo...
if not exist bin mkdir bin
javac -d bin -cp "target\fastghostmouse-1.0.0-jar-with-dependencies.jar" examples\Demo\src\main\java\fastghostmouse\Demo.java

:: Run Demo
echo [4/4] Running Demo...
echo.
java -cp "bin;target\fastghostmouse-1.0.0-jar-with-dependencies.jar" "-Djava.library.path=." fastghostmouse.demo.Demo

pause
