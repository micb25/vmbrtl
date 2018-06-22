
Dec [A] as Form

Sub TestSub([B] as Form)
 [B].Enable()
 [B].Width = 200
 [B].Height = 200
EndSub

TestSub([A])
MsgBox(Str([A].Width) + ":" + Str([A].Height), "blubb", 32)
TestSub([A])
MsgBox(Str([A].Width) + ":" + Str([A].Height), "blubb", 32)
Term()