Dec [a] as Timer

[a].Interval = 3000
[a].Enable()
repeat

until ( [a].Elapsed = false() )

MsgBox("Term()", "blubb", 32)
Term()