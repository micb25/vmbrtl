Dec [Eingabewort], [Buchstabe], [Geraten] As String 
Dec [Wortlaenge], [i], [k], [Richtig],[Falsch],[FBool],[Repeat], [GBool] As Integer
//Setzt die Konsolen Position auf x , y
Sub ConsoleSetXY([X], [Y] As Integer)
	ConsoleSetX([X]) : ConsoleSetY([Y])
EndSub
//erstellt das Menü
Sub Menue()
  ConsoleSetXY(26, 2)
  ConsoleWriteLn("Herzlich Willkommen zu Hangman!")
  ConsoleSetXY(19, 4)
  ConsoleWriteLn("Dieses Beispiel wurde in VMB'06 programmiert!")
  ConsoleSetXY(58, 23)
  ConsoleWriteLn("Copyright embyte 2006")
  ConsoleSetXY(1, 7)
EndSub
//eigentliches Programm
ConsoleCreate()
ConsoleSetName("Hangman")  //Name des Programmes
Menue()                    //Menü für Spieler1 wird erstellt
ConsoleWriteLn("  Spieler 1: geben sie das gesuchte Wort ein!")
//Eingabe des Wortes
ConsoleSetXY(14, 9)
[Eingabewort] = LCase(ConsoleReadLn())
[Wortlaenge] = Length([Eingabewort])
ConsoleClear()   //löscht die Konsole
Menue()          //Menü für Spieler 2 wird erstellt 
ConsoleWriteLn("  Spieler 2: Das gesuchte Wort hat " + Str([Wortlaenge]) + " Zeichen!")
ConsoleWriteLn("             Du hast noch 7 falsche Versuche!")
ConsoleWriteLn("")
ConsoleWrite("             ")
//Startwerte der Variablen
[i] = 0
[Geraten] = ""
[Richtig] = 0
[Falsch] = 0
[Repeat] = 0   //Boolean!!
//erstellt die Unterstriche
Repeat
  [i]++
  If (CharAt([Eingabewort], [i]) = " ") Then  // falls Leerzeichen im string auftauchen
    ConsoleWrite("  ")
    [Richtig]++
    [Geraten] = " "
  Else
    ConsoleWrite("_ ")
  EndIf
Until ( [i] >= [Wortlaenge] )
ConsoleSetXY(14, 12)
ConsoleWrite("Dein Tipp: ")

Repeat						//Prüfschleife
  ConsoleSetXY(25, 12)				//Eingabeposition
  ConsoleWrite(" ")
  ConsoleSetXY(25, 12)
  [Buchstabe] = LCase(ConsoleReadLn())	//einlesen
  If (Length([Buchstabe]) != 1) Then		//wenn mehr als ein Buchstabe eingegeben
    ConsoleSetXY(14, 14)
    ConsoleWriteLn("Bitte nur einen Buchstaben eingeben!")  
  Else
    [FBool] = 0
    [GBool] = 0
    //soll prüfen, ob der Buchstabe schon verwendet wurde
    
    If ([Geraten] != "") Then 
      For [k] = 1 To Length([Geraten])
        If (CharAt([Geraten], [k]) = [Buchstabe]) Then
         [GBool] = 1
        EndIf 
      EndFor
    EndIf
    //beginnt prüfung, ob Buchstabe im Wort vorhanden ist
    If ([GBool] = 0) Then  //wenn Buchstabe noch nicht verwendet
      [Geraten] = [Geraten] + [Buchstabe]	//alle bereits geratenen Buchstaben
      For [i] = 1 To [Wortlaenge]
        If (CharAt([Eingabewort], [i]) = [Buchstabe]) Then
          ConsoleSetXY(12 + ([i] * 2), 10)
          ConsoleWrite([Buchstabe])
          [Richtig]++
          [FBool] = 1
        EndIf
      EndFor
      ConsoleSetXY(14, 14)
      ConsoleWriteLn("Bereits geratene Buchstaben: " + [Geraten])
      If ([Fbool] = 0) Then
        [Falsch]++
        ConsoleSetXY(27, 8)
        ConsoleWrite(Str(7 - [Falsch]))
      EndIf
    Else
      ConsoleSetXY(14, 14)
      ConsoleWriteLn("Sie haben diesen Buchstaben bereits geraten!")
    EndIf
    If ([Falsch] >= 7) Then
      [Repeat] = 1
      ConsoleSetXY(14, 14)
      ConsoleWriteLn("Sie haben leider Verloren! Das gesuchte Wort war: " + [Eingabewort]) 
    Else 
      If ([Richtig] = [Wortlaenge]) Then
        [Repeat] = 1
        ConsoleSetXY(14, 14)
        ConsoleWriteLn("Sie haben das Wort korrekt erraten!                                     ")
      EndIf
    EndIf
  EndIf
Until ([Repeat] = 1)
ConsoleReadLn()
Term()
