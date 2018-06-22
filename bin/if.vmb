
ConsoleCreate()

if ( 1 = 1 ) then
  ConsoleWriteLn("CI01:0001:")
elseif ( 2 = 3) then
  ConsoleWriteLn("CI02:0001:")
else
  ConsoleWriteLn("CI01:0003:")
EndIf
Sleep(2000)
ConsoleDestroy()
Term()