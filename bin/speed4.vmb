
Sub SpeedTest([I] as Integer)
 Dec [A], [B], [C] as Integer
 [B] = 0 : [C] = 0
 [A] = GetTickCount() + 1000   
 If ( [I] = 1 ) then
  %LABEL%
     if ( GetTickCount() > [A] ) then
       [A] = GetTickCount() + 1000
       ConsoleWriteLn("cycles: " + Str([B]))
       [B] = 0
       [C]++
       If ( [C] >= 5 ) then
         Return
       EndIf
     EndIf
     [B]++
  Goto %LABEL%
 ElseIf ( [I] = 2 ) then
  For [C] = 1 To 200000
     if ( GetTickCount() > [A] ) then
       [A] = GetTickCount() + 1000
       ConsoleWriteLn("cycles: " + Str([B]))
       [B] = 0
       [C]++
     EndIf
     [B]++    
  EndFor 
  Return
 ElseIf ( [I] = 3 ) then
  repeat
     if ( GetTickCount() > [A] ) then
       [A] = GetTickCount() + 1000
       ConsoleWriteLn("cycles: " + Str([B]))
       [B] = 0
       [C]++
       If ( [C] >= 5 ) then
         Return
       EndIf
     EndIf
     [B]++
  until ( "0" = "1" )
 EndIf
EndSub

ConsoleCreate()
ConsoleWriteLn("...SPEED test: Goto")
SpeedTest(1)
ConsoleWriteLn("...SPEED test: For...EndFor")
SpeedTest(2)
ConsoleWriteLn("...SPEED test: Repeat...Until")
SpeedTest(3)
ConsoleWriteLn("that's all, folks!")  
Term()