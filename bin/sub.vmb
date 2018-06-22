
Sub ConsoleSetXY([X], [Y] As Integer)
     Dec [I] as Integer
     [I] = 123
     ConsoleSetX([X]) : ConsoleSetY([Y])
     ConsoleWriteLn([I])
EndSub

ConsoleCreate()

ConsoleSetXY(5, 5)
ConsoleWriteLn(Str(3*(1+2)))


MsgBox("2. gut", "blubb", 32)
Term()