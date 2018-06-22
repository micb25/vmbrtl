Dec [C] as Integer

Sub Fakultaet([I] as Integer) as Integer
      If ( [I] > 1 ) then
          return Fakultaet([I] - 1) * [I]
      Else
          Return 1
       EndIf
EndSub

ConsoleCreate()
%blubb%
 For [C] = 1 To 20
     ConsoleWriteLn(Str([C]) + "! = " + Str(Fakultaet([C])))
 EndFor
goto %blubb%
ConsoleReadLn()
Term()