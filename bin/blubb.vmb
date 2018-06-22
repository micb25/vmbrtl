
Dec [x], [y], [dx], [dy] as Integer

ConsoleCreate()
ConsoleSetName(" Blubb! ")
%endlos%
     ConsoleColor(Random(16))
[x] = 1 : [y] = 1 : [dx] = 1 : [dy] = 1
     ConsoleWrite(Str(Random(2)))
Goto %endlos%

ConsoleDestroy()
Term()