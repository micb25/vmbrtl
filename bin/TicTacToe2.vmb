// 
Include "ButtonTools.vmb"

Dec [Form] as Form,
    [b1], [b2], [b3], [b4], [b5], [b6], [b7], [b8], [b9] as Button

Sub SetupForm()
   Dec [X], [Y], [Z] as Integer
   [X] = 40 // Buttonaußenmaß
   [Y] = 2 //Buttonabstand
   [Z] = 4 * [y] + 3 * [X] // ergibt 4*2 + 3*40 = 128

   [Form].Height = [Z] //+ 62
   [Form].Width = [Z] // + 22
   [Form].Style = 4
   [Form].Icons = 1
   [Form].Center()
   
   [b1].Text = Str([Form].Width) : [b1].Width = [X] : [b1].Height = [X] // auf dem Button steht aber nur 124
   [b2].Text = Str([Form].Height) : [b2].Width = [X] : [b2].Height = [X] // hier sogar nur 92
   [b3].Text = "" : [b3].Width = [X] : [b3].Height = [X]
   [b4].Text = "" : [b4].Width = [X] : [b4].Height = [X]
   [b5].Text = "" : [b5].Width = [X] : [b5].Height = [X]
   [b6].Text = "" : [b6].Width = [X] : [b6].Height = [X]
   [b7].Text = "" : [b7].Width = [X] : [b7].Height = [X]
   [b8].Text = "" : [b8].Width = [X] : [b8].Height = [X]
   [b9].Text = "" : [b9].Width = [X] : [b9].Height = [X]

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

Sub AI()
EndSub

Sub b1Clicked()
  if ([b1].Text = "") then
    [b1].Text = "X"
    AI()
  endif
EndSub

Sub b2Clicked()
  if ([b2].Text = "") then
    [b2].Text = "X"
    AI()
  endif
EndSub

Sub b3Clicked()
  if ([b3].Text = "") then
    [b3].Text = "X"
    AI()
  endif
EndSub

Sub b4Clicked()
  if ([b4].Text = "") then
    [b4].Text = "X"
    AI()
  endif
EndSub

Sub b5Clicked()
  if ([b5].Text = "") then
    [b5].Text = "X"
    AI()
  endif
EndSub

Sub b6Clicked()
  if ([b6].Text = "") then
    [b6].Text = "X"
    AI()
  endif
EndSub

Sub b7Clicked()
  if ([b7].Text = "") then
    [b7].Text = "X"
    AI()
  endif
EndSub

Sub b8Clicked()
  if ([b8].Text = "") then
    [b8].Text = "X"
    AI()
  endif
EndSub

Sub b9Clicked()
  if ([b9].Text = "") then
    [b9].Text = "X"
    AI()
  endif
EndSub

Sub GetButtonEvents() //OnClickereignisse
  Repeat
     Sleep(50)

     If ([b1].Clicked = true()) then
       [b1].Clicked = false()
       b1Clicked()
     EndIf

     If ([b2].Clicked = true()) then
       [b2].Clicked = false()
       b2Clicked()
     EndIf

     If ([b3].Clicked = true()) then
       [b3].Clicked = false()
       b3Clicked()
     EndIf
     If ([b4].Clicked = true()) then
       [b4].Clicked = false()
       b4Clicked()
     EndIf

     If ([b5].Clicked = true()) then
       [b5].Clicked = false()
       b5Clicked()
     EndIf

     If ([b6].Clicked = true()) then
       [b6].Clicked = false()
       b6Clicked()
     EndIf

     If ([b7].Clicked = true()) then
       [b7].Clicked = false()
       b7Clicked()
     EndIf

     If ([b8].Clicked = true()) then
       [b8].Clicked = false()
       b8Clicked()
     EndIf

     If ([b9].Clicked = true()) then
       [b9].Clicked = false()
       b9Clicked()
     EndIf

  Until ( [Form].Visible = false() )
EndSub



Sub GameReset()
   // TODO: Spiel zurücksetzen
EndSub

SetupForm()
GetButtonEvents()
GameReset()

Repeat
  Sleep(50)
Until ( [Form].Visible = false() )
Term()
