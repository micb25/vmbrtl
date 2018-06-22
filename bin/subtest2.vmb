
Include "Windows.vmb"

Dec [I] as Integer

Extern("kernel32.dll", "GetTickCount") Sub _GetTickCount() As Integer

ConsoleCreate()
ConsoleWriteLn("BUILTIN: " + Str(GetTickCount()))
ConsoleWriteLn("EXTERN : " + Str(_GetTickCount()))
ConsoleWriteLn("WinDir: " + GetWinDir())
[I] = malloc(256)
MemWriteByte([I], 65)
ConsoleWriteLn(Str(MemReadByte([I])))

Sleep(2000)
Term()