// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

// Put your code here.

	@i 			// i: screen offset
	M=0

(LOOP)
	@KBD
	D=M
	@PAINT
	D;JNE 		// if read something, jump to PAINT
	@CLEAR
	0;JMP 		// otherwise, jump to CLEAR
	@LOOP
	0;JMP 		// loops

(PAINT)
	@i 			// if offset >= 8192, skip PAINT (whole screen is already painted)
	D=M
	@8192
	D=D-A
	@LOOP
	D;JGE

	@i 			// SCREEN[i] = -1 
	D=M
	@SCREEN
	A=A+D
	M=-1	
	@i 			// i++
	M=M+1

	@LOOP 		// return 
	0;JMP

(CLEAR)
	@SCREEN 	// SCREEN[0] = 0
	M=0
	@i 			// if offset == 0, skip clear (whole screen is cleared)
	D=M
	@LOOP
	D;JEQ

	@i 			// SCREEN[i] = 0
	D=M
	@SCREEN
	A=A+D
	M=0
	@i 			// i--
	M=M-1

	@LOOP 		// return
	0;JMPs