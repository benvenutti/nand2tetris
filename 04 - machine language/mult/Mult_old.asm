// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[3], respectively.)

// Put your code here
	
	// R2=0
	@0
	D=A
	@R2
	M=D

	// if R0=0 jump to end
	@R0
	D=M
	@END
	D; JEQ

	// if R1=0 jump to end
	@R1
	D=M
	@END
	D; JEQ

	@i
	M=1

	@R1
	D=M
	@R2
	M=D

(LOOP)

	@i
	D=M
	@R0
	D=D-M
	@END
	D; JGE

	@i
	M=M+1

	@R1
	D=M
	@R2
	M=M+D

	@LOOP
	0; JMP

(END)

	@END
	0; JMP
