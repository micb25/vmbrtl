@echo off
cd bin
@del *.exe
@del *.txt
cd ..
cd obj
@del *.* /Y
cd ..