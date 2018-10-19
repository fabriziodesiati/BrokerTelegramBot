echo OFF
echo Copy BrokerTelegramBot on Release Path
echo  param1 is BUIDIR (default C:\Projects\PERSONAL\BUILDS)
echo  param2 is RELDIR (default C:\Projects\PERSONAL\RELEASE\BrokerTelegramBot\Release)
echo  param3 is BUILDTYPE [Release,Debug,RelWithDebInfo] (default Release)

set "DEPLOY_EXE=BrokerTelegramBot"

if /I "%1" equ "" ( 
  set BUIDIR=C:\Projects\PERSONAL\BUILDS
) else ( 
  set BUIDIR=%1
)

if /I "%2" equ "" ( 
  set RELDIR=C:\Projects\PERSONAL\RELEASE\BrokerTelegramBot\Release
) else ( 
  set RELDIR=%2
)

if /I "%3" equ "" (
  set TYPE=Release
) else (
  set TYPE=%3
)

set DBG=
if /I "%TYPE%" equ "Debug" (
  set DBG=d
)

set      SYSTEM32DIR="%windir%\System32"

if /I "%4" neq "" (
  echo --- COPY External Dependencies:
  echo      SYSTEM32DIR=%SYSTEM32DIR%
  
  echo --- COPY Microsoft Visual C++ 2015 Redistributable x64:
  echo --- ...copy msvcp140%DBG%.dll
  copy %SYSTEM32DIR%\msvcp140%DBG%.dll                                        %RELDIR%\.
  echo --- ...copy vcruntime140%DBG%.dll
  copy %SYSTEM32DIR%\vcruntime140%DBG%.dll                                    %RELDIR%\.
)

echo BUIDIR=%BUIDIR%
echo RELDIR=%RELDIR%

::DEPLOY_EXE
echo --- COPY EXECUTABLES:
set PREV_DIR=%cd%
cd %BUIDIR%
for %%G in (%DEPLOY_EXE%) do (
  echo --- ...copy %%G.exe
  copy %BUIDIR%\%%G\%TYPE%\%%G.exe %RELDIR%\. 
  
)
cd  %PREV_DIR%

::WINDEPLOYQT
set WINDEPLOYQT="C:\Qt\Qt5.11.1\5.11.1\msvc2017_64\bin\windeployqt.exe"
if /I "%4" neq "" (
  echo WINDEPLOYQT=%WINDEPLOYQT%
  echo --- DEPLOY QT:
  echo %WINDEPLOYQT% "%RELDIR%\BrokerTelegramBot.exe" --dir="%RELDIR%"
  %WINDEPLOYQT% "%RELDIR%\BrokerTelegramBot.exe" --dir="%RELDIR%"
)

pause
