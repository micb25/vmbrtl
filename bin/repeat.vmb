Dec [a] as Timer

[a].Interval = 3000
[a].Enable()
repeat

until ( [a].Elapsed = true() )

MsgBox("Term()", "blubb", 32)
Term()