Dec [i1], [i2] as Integer
[i1] = 1
[i2] = 3
ConsoleCreate()
if ( [i1] = 1 ) then
  ConsoleWriteLn("CI01:0001:")
  if ( [i2] = 3 ) then 
    consolewriteln("verschachtelung")
  Endif

elseif ( [i1] = 3) then
  ConsoleWriteLn("CI02:0001:")
else
  ConsoleWriteLn("CI01:0003:")
EndIf
Sleep(2000)
ConsoleDestroy()
Term()