Dec [Form] as Form, [B] as Array(Button, 9)
Dec [X], [Y] as Integer

for [x] = 1 To 3
     for [y] = 0 To 2
          [B](3*[Y]+[X]).Text = Str(3*[Y]+[X])
          [B](3*[Y]+[X]).X = [X] * 40 - 35
          [B](3*[Y]+[X]).Y = [Y] * 40 + 5
          [B](3*[Y]+[X]).Width = 30
          [B](3*[Y]+[X]).Height = 30
     EndFor     
EndFor

[Form].Show()

repeat
     For [X] = 1 To 3
          For [Y] = 0 To 2
               If ( [B](3*[Y]+[X]).Clicked ) then
                    [B](3*[Y]+[X]).Text = "X"
                    [B](3*[Y]+[X]).Disable()
               EndIf
          EndFor          
     EndFor
     Sleep(25)
until ( [Form].Closed )
Term()