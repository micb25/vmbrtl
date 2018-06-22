Dec [Form] as Form, [Button] as Button

[Form].Width = 640
[Form].Height = 480
[Form].Center()

[Button].X = 192
[Button].Y = 160
[Button].Width = 257 
[Button].Height = 97
[Button].Text = "Beenden"

[Form].Show()
repeat
     If ( [Button].Clicked = true() ) then
       MsgBox("Dies ist nur ein Test!", "Testfenster", 32)
       Term()
     EndIf
//      Sleep(50)
until ( [Form].Visible = false() )
Term()