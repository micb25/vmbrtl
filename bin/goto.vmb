Dec [A] as String
ConsoleCreate()

[A] = "ELM!"
Goto %Write1%
%MIB%
[A] = "MiB!"
Goto %Write2%

ConsoleWriteLn("L000:0001")

%Write1%
ConsoleWriteLn("Hallo " + [A])
Goto %MiB%

%Write2%
ConsoleWriteLn("Hallo " + [A])

Sleep(1000)
ConsoleDestroy()
Term()