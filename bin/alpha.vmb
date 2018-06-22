Dec [a],[b],[c],[d] as Form

[a].X = 10
[b].X = 10
[c].X = 10
[d].X = 10

[a].height = 100
[b].height = 100
[c].height = 100
[d].height = 100

[a].Y = 0
[b].Y = 200
[c].Y = 400
[d].Y = 600

[a].Alphablend = true()
[a].AlphaValue = 255

[b].Alphablend = true()
[b].AlphaValue = 200
[b].Color = 0;

[c].Color = 0;

[d].Transparent = true()
[d].TransColor = 0
[d].Color = 0

[a].Show()
[b].Show()
[c].Show()
[d].Show()

Sleep(5000)
Term()