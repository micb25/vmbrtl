Dec [Form] as Form, [B] as Edit, [C] as Edit

[Form].Show()
[B].X = 50
[B].Y = 50
[B].Width = 100
[B].Height = 30
[B].Font = "Arial"
[B].FontStyle = 1
[B].Color = 255
[B].FontSize = 10
[B].PassChar = "#"

[C].X = 50
[C].Y = 80
[C].Width = 100
[C].Height = 30
[C].Font = "Arial"
[C].FontStyle = 1
[C].Color = 255
[C].FontSize = 10

// [Form].CanClose = false()
repeat

//      if ( [B].Clicked = true() ) then
//           MsgBox("du sau ;)", "angeklickt!", 16)
//           [B].ClearEvents()
//      endif
     [C].Text = [B].text
     Sleep(50)
until ([Form].Visible = false() )

Term()
