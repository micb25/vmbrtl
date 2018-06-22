Dec [Form] as Form

[Form].Show
[Form].Text = "Blubb123"
[Form].Color = 255
Dec [Form2] as Form

[Form2].Visible = true()

[Form].X = 32 : [Form].Y = 30 : [Form].Width = 250 : [Form].Height = 100
MsgBox("Hallo!", [Form].Text, [Form].X)
Sleep(3000) 
Term()
