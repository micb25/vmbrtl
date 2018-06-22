Dec [Farbe] as Integer
ConsoleCreate()
For [Farbe] = 1 To 15
  ConsoleColor([Farbe])
  ConsoleWriteLn("Ich mag es bunt!")
EndFor
Sleep(2000)
Term()