Dec [Form] as Form, [i] as Integer

[Form].Show
[Form].Text = "Blubb"
[Form].Color = 255
[Form].X = 32 : [Form].Y = 30 : [Form].Width = 250 : [Form].Height = 100

For [i] = 1 To 500
  [Form].X = [I]
EndFor

Term()
