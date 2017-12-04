@echo off
set pathh = "C:\Users\Adi\Desktop\Ilay\PROJECTS\CPP-PROJECTS\TicTacTOR\exes"

start "middle node 2" C:\Users\Adi\Desktop\Ilay\PROJECTS\CPP-PROJECTS\TicTacTOR\exes\Node2.exe
echo "Started middle node 2"

start "middle node" C:\Users\Adi\Desktop\Ilay\PROJECTS\CPP-PROJECTS\TicTacTOR\exes\Node3.exe
echo "Started middle node 1"

start "end node" C:\Users\Adi\Desktop\Ilay\PROJECTS\CPP-PROJECTS\TicTacTOR\exes\Node1.exe
echo "Started end node"

start "Started start node" C:\Users\Adi\Desktop\Ilay\PROJECTS\CPP-PROJECTS\TicTacTOR\exes\Node0.exe
echo "Started start node"

PAUSE

