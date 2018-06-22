Dec [Str] as String

[Str] = "Hallo!"
ConsoleCreate()
ConsoleColor(11)
ConsoleWriteLn(RightStr(Str(12356), 0))
ConsoleWriteLn(RightStr(Str(12356), 1))
ConsoleWriteLn(RightStr(Str(12356), 2))
ConsoleWriteLn(RightStr(Str(12356), 3))
ConsoleWriteLn(RightStr(Str(12356), 4))
ConsoleWriteLn(RightStr(Str(12356), 5))
ConsoleWriteLn(RightStr(Str(12356), 6))
ConsoleWriteLn(RightStr([Str], 7))
ConsoleWriteLn(RightStr([Str], 8))
Sleep(1000)
ConsoleDestroy()
Term()
