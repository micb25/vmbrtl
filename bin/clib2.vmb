Dec [Str] as String
Dec [Form] as Form

[Form].Center
[Str] = "Hallo " + "Welt!"
ConsoleCreate()
ConsoleSetName("Hallo Welt!")
ConsoleSetX(20)
ConsoleSetY(10)
ConsoleColor(10)
ConsoleWrite("Hallo! Wie heisst du? ")
[Str] = ConsoleReadLn()
ConsoleWriteLn("")
ConsoleSetX(20)
// ConsoleWriteLn("Hallo " + Char(34) + UCase([Str]) + Char(34))
ConsoleWrite("Hallo " + Char(34) + UCase([Str]) + Char(34) + "!")
Sleep(1000)
ConsoleDestroy()
Term()
