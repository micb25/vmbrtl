
Include "ButtonTools.vmb"

Dec [Form] as Form,
    [b1], [b2], [b3], [b4], [b5], [b6], [b7], [b8], [b9] as Button,
    [SillyAI] as Bool,
    [ClickNr] as Integer

// Sub GetAppPath() as String
//      Dec [S] as String, [I], [P] as Integer
//      [S] = ParamStr(0)
//      For [I] = 1 To Length([S])
//           If ( CharAt([S], [I]) = "\" ) then
//             [P] = [I]
//           EndIf
//      EndFor
//      If ( [P] > 0 ) then
//        [S] = MidStr([S], 2, [P] - 1)
//      EndIf
//      Return [S]
// EndSub

Sub GameReset()
  [b1].Text = ""
  [b2].Text = ""
  [b3].Text = ""
  [b4].Text = ""
  [b5].Text = ""
  [b6].Text = ""
  [b7].Text = ""
  [b8].Text = ""
  [b9].Text = ""
  [ClickNr] = 0
EndSub

Sub SetupForm()
  Dec [X], [Y], [Z] as Integer,
      [Spieler1], [Spieler2] as String
  [X] = 40 // Buttonaußenmaß
  [Y] = 2 //Buttonabtsand
  [Z] = 4 * [y] + 3 * [X]
  [SillyAI] = true()
  [Form].Style = 4
  [Form].ClientHeight = [Z]
  [Form].ClientWidth = [Z]
  [Form].Text = "TicTacToe"
  [Form].Center()
  [Form].Icons = 1
  GameReset()

  [b1].Width = [X] : [b1].Height = [X]
  [b2].Width = [X] : [b2].Height = [X]
  [b3].Width = [X] : [b3].Height = [X]
  [b4].Width = [X] : [b4].Height = [X]
  [b5].Width = [X] : [b5].Height = [X]
  [b6].Width = [X] : [b6].Height = [X]
  [b7].Width = [X] : [b7].Height = [X]
  [b8].Width = [X] : [b8].Height = [X]
  [b9].Width = [X] : [b9].Height = [X]
  
//   MakeBitmapButton([b1].Handle, GetAppPath() + "O.BMP")

  [b1].X = [Y]               : [b1].Y = [Y]
  [b2].X = [X] + 2* [Y]      : [b2].Y = [Y]
  [b3].X = 2 * [X] + 3 * [Y] : [b3].Y = [Y]
  [b4].X = [Y]               : [b4].Y = [X] + 2* [Y]
  [b5].X = [X] + 2* [Y]      : [b5].Y = [X] + 2* [Y]
  [b6].X = 2 * [X] + 3 * [Y] : [b6].Y = [X] + 2* [Y]
  [b7].X = [Y]               : [b7].Y = 2 * [X] + 3 * [Y]
  [b8].X = [X] + 2* [Y]      : [b8].Y = 2 * [X] + 3 * [Y]
  [b9].X = 2 * [X] + 3 * [Y] : [b9].Y = 2 * [X] + 3 * [Y]

  [Form].Show()
EndSub

Sub CheckForWin() As Bool
  Dec [win] as String
  
  [win] = ""
  If ([b1].Text = [b4].Text) And ([b1].Text = [b7].Text) And ([b1].Text != "") Then
    [win] = [b1].Text
  ElseIf ([b2].Text = [b5].Text) And ([b2].Text = [b8].Text) And ([b2].Text != "") Then
    [win] = [b2].Text
  ElseIf ([b3].Text = [b6].Text) And ([b3].Text = [b9].Text) And ([b3].Text != "") Then
    [win] = [b3].Text
  ElseIf ([b1].Text = [b2].Text) And ([b1].Text = [b3].Text) And ([b1].Text != "") Then
    [win] = [b1].Text
  ElseIf ([b4].Text = [b5].Text) And ([b4].Text = [b6].Text) And ([b4].Text != "") Then
    [win] = [b4].Text
  ElseIf ([b7].Text = [b8].Text) And ([b7].Text = [b9].Text) And ([b7].Text != "") Then
    [win] = [b7].Text
  ElseIf ([b1].Text = [b5].Text) And ([b1].Text = [b9].Text) And ([b1].Text != "") Then
    [win] = [b1].Text
  ElseIf ([b3].Text = [b5].Text) And ([b3].Text = [b7].Text) And ([b3].Text != "") Then
    [win] = [b3].Text
  EndIf
  if ([win] != "") then
    MsgBox("Spieler " + [win] + " hat gewonnen!", "Spielende", 0)
    GameReset()
    return true()
  ElseIf ([ClickNr] >= 5) then
    MsgBox("Unentschieden!", "Spielende", 0)
    GameReset()
  Else
    return false()
  EndIf  
EndSub

Sub AI() as Bool
  If (CheckForWin() = false()) then
    If ([SillyAI] = true()) then
      Dec [Pos] as Integer
      [Pos] = Random(9)+1
      If ([Pos] = 1) And ([b1].Text = "") then
        [b1].Text = "O"
      ElseIf ([Pos] = 2) And ([b2].Text = "") then
        [b2].Text = "O"
      ElseIf ([Pos] = 3) And ([b3].Text = "") then
        [b3].Text = "O"
      ElseIf ([Pos] = 4) And ([b4].Text = "") then
        [b4].Text = "O"
      ElseIf ([Pos] = 5) And ([b5].Text = "") then
        [b5].Text = "O"
      ElseIf ([Pos] = 6) And ([b6].Text = "") then
        [b6].Text = "O"
      ElseIf ([Pos] = 7) And ([b7].Text = "") then
        [b7].Text = "O"
      ElseIf ([Pos] = 8) And ([b8].Text = "") then
        [b8].Text = "O"
      ElseIf ([Pos] = 9) And ([b9].Text = "") then
        [b9].Text = "O"
      Else
        AI()
      EndIf
      CheckForWin()
    EndIf
  EndIf
EndSub

Sub FieldClicked([bx] as Button)
  [bx].Clicked = false()
  if  ([bx].Text = "") then
    [ClickNr] = [ClickNr] + 1
    [bx].Text = "X"
    AI() 
  EndIf
EndSub


Sub GetButtonEvents() //OnClickereignisse
  Repeat
    Sleep(50)

    If ([b1].Clicked = true()) then
      FieldClicked([b1])
    EndIf

    If ([b2].Clicked = true()) then
      FieldClicked([b2])
    EndIf

    If ([b3].Clicked = true()) then
      FieldClicked([b3])
    EndIf

    If ([b4].Clicked = true()) then
      FieldClicked([b4])
    EndIf

    If ([b5].Clicked = true()) then
      FieldClicked([b5])
    EndIf

    If ([b6].Clicked = true()) then
      FieldClicked([b6])
    EndIf

    If ([b7].Clicked = true()) then
      FieldClicked([b7])
    EndIf

    If ([b8].Clicked = true()) then
      FieldClicked([b8])
    EndIf

    If ([b9].Clicked = true()) then
      FieldClicked([b9])
    EndIf

  Until ( [Form].Visible = false() )
EndSub

//Programmstart
SetupForm()
GetButtonEvents()
Term()
