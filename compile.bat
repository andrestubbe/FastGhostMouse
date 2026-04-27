@echo off
set JAVA_HOME=C:\Program Files\Java\jdk-25
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cl.exe /LD /std:c++20 /EHsc /Fe:build\fastghostmouse.dll native\FastGhostMouse.cpp /I"%JAVA_HOME%\include" /I"%JAVA_HOME%\include\win32" user32.lib gdi32.lib gdiplus.lib msimg32.lib d3d11.lib d2d1.lib dcomp.lib /link /DEF:native\fastghostmouse.def
if %errorlevel% neq 0 exit /b %errorlevel%
copy build\fastghostmouse.dll src\main\resources\native\
copy build\fastghostmouse.dll .\
echo Compilation complete
