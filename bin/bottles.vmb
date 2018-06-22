Dec [Flaschen] as Integer

[Flaschen] = 99

ConsoleCreate()
ConsoleSetName("99 Bottles of Beer")
%schleife%
ConsoleWriteln(Str([Flaschen]) + " bottle(s) of beer on the wall,")
ConsoleWriteln(Str([Flaschen]) + " bottle(s) of beer.")
ConsoleWriteln("Take one down and pass around.")
[Flaschen]--
If ([Flaschen] != 0 ) then
 Goto %schleife%
Endif

ConsoleWriteln("No more bottles of beer on the wall")
