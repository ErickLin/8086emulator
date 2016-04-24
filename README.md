# 8086emulator
An Intel 8086 CPU emulator written for CS3210, our OS class. *In progress*

Dependencies:
-SDL
	On Ubuntu just run: sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev

How to run:

make
./8086emulator [prog-name] [0-1]

[prog-name] should be a .com file. Executing is safe because we have blocked support for features related to hard disk access. If [prog-name] is left blank, an SDL test will be run instead. The last argument controls the type of output that you want to see (1 by default for the parser's output, 0 for the executed COM file's output).
