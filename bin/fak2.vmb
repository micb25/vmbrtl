
Dec [C] as Integer

sub f3([i] as integer) as Integer
     return [i]-1
endsub

sub f2([i] as integer) as Integer
     return f3([i]+1)
endsub

Sub f1([i] as integer) as Integer
     return f3([i])
EndSub

ConsoleCreate()

ConsoleWriteLn(Str(f1(1337)))

ConsoleReadLn()
Term()