Dec [A] as String

ConsoleCreate()
[A] = ClipBoardGet()
ConsoleWriteLn("ClipBoard: " + [A])
ClipBoardSet("Dies ist ein Test der SetClipboard-Funktion!")
Sleep(5000)
ClipBoardSet([A])
ConsoleDestroy()
Term()
