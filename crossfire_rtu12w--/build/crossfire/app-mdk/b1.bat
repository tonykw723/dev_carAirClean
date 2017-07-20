md ..\merge_out
cd ..\merge_out
del boot.hex
del crossfire.hex
copy /y ..\boot\mdk\out\boot.hex 
copy /y ..\app-mdk\out\crossfire.hex 
::cmd.exe /c ..\..\..\util\HEX2BIN.EXE .\boot.hex 
::cmd.exe /c ..\..\..\util\HEX2BIN.EXE .\crossfire.hex 



 

