Dec [a], [b], [z1] as Integer

ConsoleCreate()
ConsoleSetName("Speed2")
[z1] = GetTickCount()
for [a] = 1 to 10000
     [b] = [a] + 1 * [a]
//      [b] = GetTickCount()
endfor
ConsoleWriteLn("Zeit: " + Str(GetTickCount() - [z1]) + "ms")
Sleep(3000)
Term()
