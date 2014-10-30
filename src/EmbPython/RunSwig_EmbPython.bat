@echo off
:: ***********************************************************************************
::  File:
::      RunSwig_EmbPython.bat
::
::  Description:
::      ファイルの依存関係を調べて、EmbPythonSwig.bat を最適に実行する.
::
::	    実行するプロジェクトは ..\..\src\RunSwig\do_swigall.projs に定義されている
::      ものを使用する. ただしプロジェクト Base は定義の有無に関わりなく実行する.
::
:: ***********************************************************************************
::  Version:
::	    Ver 1.0	  2014/10/29	F.Kanehori  初版
:: ***********************************************************************************
setlocal enabledelayedexpansion
set PROG=%~n0
set CWD=%cd%
set DEBUG=0

:: ----------
::  各種定義
:: ----------
:: ディレクトリの定義
::
set BASEDIR=..\..
set BINDIR=%BASEDIR%\bin
set SRCDIR=%BASEDIR%\src
set INCDIR=%BASEDIR%\include
set ETCDIR=%SRCDIR%\RunSwig

set EMBPYTHONDIR=.

:: 依存関係にはないと見做すファイルの一覧
::
set EXCLUDES=

:: makefile に出力するときのパス
set INCDIROUT=..\..\include
set SRCDIROUT=..\..\src

:: 使用するファイル名
::
set PROJFILE=do_swigall.projs
set MAKEFILE=Makefile_EmbPython.swig

:: 使用するプログラム名
::
set MAKE=nmake
set SWIG=EmbPythonSwig.bat

:: 使用するパス
::
set X32=c:\Program Files
set X64=c:\Program Files (x86)
set ARCH=
if exist "%X32%" set ARCH=%X32%
if exist "%X64%" set ARCH=%X64%
set VSVER=
if exist "%ARCH%\Microsoft Visual Studio 10.0" set VSVER=10.0
if exist "%ARCH%\Microsoft Visual Studio 12.0" set VSVER=12.0
set MAKEPATH="%ARCH%\Microsoft Visual Studio %VSVER%\VC\bin"
if not exist %MAKEPATH% (
    echo %PROG%: can not find '%MAKE%' path.
    exit /b
)
rem set SWIGPATH=%BINDIR%\swig
set SWIGPATH=%EMBPYTHONDIR%
set PATH=.;%SWIGPATH%;%MAKEPATH%;%PATH%

:: ------------------------------
::  処理するモジュール一覧を作成
:: ------------------------------
set PROJECTS=Base
for /f "tokens=1,*" %%m in (%ETCDIR%\%PROJFILE%) do set PROJECTS=!PROJECTS! %%m
if %DEBUG% == 1 echo Projects are: %PROJECTS%

:: ----------
::  処理開始
:: ----------
for %%p in (%PROJECTS%) do (
    echo   Project: %%p
    set MKFILE=%MAKEFILE%.%%p
    call :collect_headers %%p
    call :make_makefile %%p !MKFILE! "!INCHDRS!" "!SRCHDRS!"
    cmd /c %MAKE% -f !MKFILE!
)

:: ----------
::  処理終了
:: ----------
endlocal
exit /b

:: -----------------------------------------------------------------------------------
::  引数1 で与えられたプロジェクトのヘッダ情報を収集する
:: -----------------------------------------------------------------------------------
:collect_headers
    set PROJECT=%1

    :: 依存ファイル情報を集める
    ::
    set INCHDRS=
    for %%f in (%INCDIR%\%PROJECT%\*.h) do (
        call :one_of "%EXCLUDES%" %%~nxf
        if "!$result!" equ "no" set INCHDRS=!INCHDRS! %%~nxf
    )
    call :add_prefix "!INCHDRS:~1!" %INCDIROUT%\%PROJECT%
    set INCHDRS=%$string:\=/%
    if %DEBUG% == 1 echo INCHDRS [%INCHDRS%]

    set SRCHDRS=
    for %%f in (%SRCDIR%\%PROJECT%\*.h) do (
        call :one_of "%EXCLUDES%" %%~nxf
        if "!$result!" equ "no" set SRCHDRS=!SRCHDRS! %%~nxf
    )
    call :add_prefix "!SRCHDRS:~1!" %SRCDIROUT%\%PROJECT%
    set SRCHDRS=%$string:\=/%
    if %DEBUG% == 1 echo SRCHDRS [%SRCHDRS%]

exit /b

:: -----------------------------------------------------------------------------------
::  makefile を作成する
::      引数1   モジュール名
::      引数2   makefile 名
::      引数3   "依存ヘッダファイル名リスト"
::      引数4   "依存ソースファイル名リスト"
:: -----------------------------------------------------------------------------------
:make_makefile
    setlocal enabledelayedexpansion
    set MODULE=%1
    set MKFILE=%2
    set INCFILES=%~3
    set SRCFILES=%~4
    if %DEBUG% == 1 (
        echo MODULE   [%MODULE%]
        echo MKFILE   [%MKFILE%]
        echo INCFILES [%INCFILES%]
        echo SRCFILES [%SRCFILES%]
    )

    set TARGET=%EMBPYTHONDIR%/EP%MODULE%.cpp
    set DEPENDENCIES=%INCFILES% %SRCFILES%
    if %DEBUG% == 1 (
        echo TARGET       [%TARGET%]
        echo DEPENDENCIES [%DEPENDENCIES%]
    )

    echo #  Do not edit. RunSwig_EmbPython.bat will update this file.   > %MKFILE%
    echo #  File: %MKFILE%  >> %MKFILE%
    echo.                   >> %MKFILE%
    echo all:	%TARGET%    >> %MKFILE%
    echo %TARGET%:	%DEPENDENCIES%  >> %MKFILE%
    echo.	call %SWIG% %MODULE%    >> %MKFILE%
    echo.                   >> %MKFILE%
    for %%f in (%DEPENDENCIES%) do (
        echo %%f:	    >> %MKFILE%
    )
    endlocal
exit /b

:: -----------------------------------------------------------------------------------
::  引数2 で与えられた名前が 引数1 で与えられたリスト中にあるか調べる
::  結果は $result に返す（yes または no）
:: -----------------------------------------------------------------------------------
:one_of
    set $result=no
    for %%f in (%~1) do (if "%2" equ "%%f" (set $result=yes&& exit /b))
exit /b

:: -----------------------------------------------------------------------------------
::  引数1 で与えられた変数に、引数2 で指定された prefix を追加する
::  結果は $string に返す
:: -----------------------------------------------------------------------------------
:add_prefix
    set $string=
    for %%f in (%~1) do (set $string=!$string! %2\%%f)
    set $string=%$string:~1%
exit /b

::end RunSwig_EmbPython.bat
