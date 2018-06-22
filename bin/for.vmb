Dec [A], [B], [C] as Integer

ConsoleCreate()

 For [B] = 10 DownTo 1
  For [A] = 1 To 10
   ConsoleWriteLn( "   " + Str([A]))
  EndFor
  Next
  ConsoleWriteLn("Nooo!")
 EndFor
Sleep(1000)
ConsoleDestroy()
Term()