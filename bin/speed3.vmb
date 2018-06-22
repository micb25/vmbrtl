Dec [A], [B] as Integer

ConsoleCreate()
[A] = GetTickCount() + 1000
[B] = 0
 %Schleife%
    If ( GetTickCount() > [A] ) then
        ConsoleWriteLn("cycles: " + [B])
        [B] = 0
        [A] = GetTickCount() + 1000
    EndIf 
    [B]++
 Goto %Schleife%
