@echo off
rem ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
rem <<Settings>>

set MODULE=MyEPApp
set BIN=..\..\..\..\bin
set INCLUDE=../../../../include

rem ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

set PATH=%BIN%;%BIN%\swig

call swig.exe -cpperraswarn -sprpy -c++ %MODULE%.i & if errorlevel 1 echo !!!!SWIG FAILED!!!!! & @pause

ren EP%MODULE%.cpp EP%MODULE%.cpp.tmp

echo #include "%INCLUDE%/Springhead.h"                 > EP%MODULE%.cpp
echo #include "%INCLUDE%/Python/Python.h"             >> EP%MODULE%.cpp
echo #include "%INCLUDE%/EmbPython/SprEPUtility.h"    >> EP%MODULE%.cpp
echo #include "%INCLUDE%/EmbPython/SprEPBase.h"       >> EP%MODULE%.cpp
echo #include "%INCLUDE%/EmbPython/SprEPFoundation.h" >> EP%MODULE%.cpp
echo #include "%MODULE%.h"                            >> EP%MODULE%.cpp
echo #pragma warning(disable:4244)                    >> EP%MODULE%.cpp

type EP%MODULE%.cpp.tmp >> EP%MODULE%.cpp
del  EP%MODULE%.cpp.tmp

