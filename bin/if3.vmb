
ConsoleCreate()

if ( 1 = 1 ) and ( 1 = 1) then
     ConsoleWriteLn("richtig!1")
endif

if ( 0 = 1 ) and ( 0 = 1) then
     ConsoleWriteLn("falsch!1")
endif

if ( 0 = 1 ) and ( 1 = 1) then
     ConsoleWriteLn("falsch!1")
endif

if ( 1 = 1 ) and ( 0 = 1) then
     ConsoleWriteLn("falsch!1")
endif

if ( 1 = 1 ) or ( 0 = 1) then
     ConsoleWriteLn("richtig!2")
endif

if ( 0 = 1 ) or ( 0 = 1) then
     ConsoleWriteLn("falsch!2")
endif

if ( 0 = 1 ) or ( 1 = 1) then
     ConsoleWriteLn("richtig!2")
endif

if ( 1 = 1 ) or ( 1 = 1) then
     ConsoleWriteLn("richtig!2")
endif

if ( 1 = 1 ) xor ( 0 = 1) then
     ConsoleWriteLn("richtig!3")
endif

if ( 0 = 1 ) xor ( 0 = 1) then
     ConsoleWriteLn("falsch!3")
endif

if ( 0 = 1 ) xor ( 1 = 1) then
     ConsoleWriteLn("richtig!3")
endif

if ( 1 = 1 ) xor ( 1 = 1) then
     ConsoleWriteLn("falsch!3")
endif

if ( 1 = 1 ) nor ( 0 = 1) then
     ConsoleWriteLn("falsch!4")
endif

if ( 0 = 1 ) nor ( 0 = 1) then
     ConsoleWriteLn("richtig!4")
endif

if ( 0 = 1 ) nor ( 1 = 1) then
     ConsoleWriteLn("falsch!4")
endif

if ( 1 = 1 ) nor ( 1 = 1) then
     ConsoleWriteLn("falsch!4")
endif

if ( 1 = 1 ) nand ( 1 = 1) then
     ConsoleWriteLn("falsch!5")
endif

if ( 0 = 1 ) nand ( 0 = 1) then
     ConsoleWriteLn("richtig!5")
endif

if ( 0 = 1 ) nand ( 1 = 1) then
     ConsoleWriteLn("richtig!5")
endif

if ( 1 = 1 ) nand ( 0 = 1) then
     ConsoleWriteLn("richtig!5")
endif

Sleep(2000)
ConsoleDestroy()
Term()
