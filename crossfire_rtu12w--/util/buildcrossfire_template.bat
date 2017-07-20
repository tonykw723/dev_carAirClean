@echo off & setlocal EnableDelayedExpansion

set UV4_PATH="C:\Keil\UV4"
set SVN_PATH="C:\Program Files\VisualSVN Server\bin"

set pathFile="E:\iotBuildCmd\crossfire\crossfiresvnPath.txt"
set outPath="E:\iotBuildBin\crossfire"
set buildPath="crossfire_build_temp"

set BAKFILE=crossfire_L1xx_bak.uvproj
set src=0x8002000
set dst=0x8011000

:loop

if exist %pathFile% (
	echo "hello!"
	echo !pathFile!
	set /p svnPath=<%pathFile%
	echo !svnPath!
	%SVN_PATH%\svn co !svnPath! %buildPath%
	
	cd %buildPath%\build\crossfire\app-mdk
	
	if exist %BAKFILE% (del /f/q %BAKFILE%)
  for /f "usebackq tokens=*" %%j in ("crossfire_L1xx.uvproj") do (
    set TMP=%%j
    echo !TMP:%src%=%dst%!>nul 1>>%BAKFILE%
  )
  %UV4_PATH%\Uv4 -b crossfire_L1xx_bak.uvproj
  move .\out\crossfire.bin .\out\cf.bin
  move .\out\crossfire.hex .\out\cf.hex
  
	%UV4_PATH%\Uv4 -b crossfire_L1xx.uvproj

	cd ..\..\..\..\
	
	cd %buildPath%\build\crossfire\boot\mdk
	%UV4_PATH%\Uv4 -b boot_L1xx.uvproj
	cd ..\..\..\..\..\
  
	cd %buildPath%
	%SVN_PATH%\svn info >>cfossfire_svn_info.txt
	findstr "Revision" cfossfire_svn_info.txt >>cfossfire_svn.txt
	for /f "tokens=2" %%a in (cfossfire_svn.txt) do set binSvn=%%a
	echo !binSvn!
	set binPath=%outPath%\%date:~5,2%%date:~8,2%-!binSvn!
	echo !binPath!
	md !binPath!
	
	copy /y .\build\crossfire\app-mdk\out\*.hex !binPath! 
	copy /y .\build\crossfire\app-mdk\out\*.bin !binPath!
	copy /y .\build\crossfire\boot\mdk\out\*.hex !binPath!
	cd ..
	
	cd %buildPath%\util\rtuReleaseTool\binary
	md nand
	copy /y ..\..\..\build\crossfire\app-mdk\out\*.bin nand
	copy /y ..\..\..\build\crossfire\app-mdk\out\*.ini nand
	.\rtuReleaseTool.exe nand !binSvn!
	copy /y .\*.z !binPath!
	rem copy /y .\*.pkg !binPath!
	copy /y .\nand\*.ini !binPath!
	cd ..\..\..\..
	
	rd /s/q %buildPath%
	del %pathFile%
) else (
	echo "path file not exist!" 
)

goto delay

:delay
echo off
rem echo before:%time%
for /l %%i in (1,1,100000) do echo %%i>nul
rem echo after :%time%
goto loop

:end
