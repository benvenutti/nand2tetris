// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[3], respectively.)

// Put your code here.

	@i 			// i = 1 (counter)
	M=1
	@R2 		// R2 = 0
	M=0

(LOOP)
	@i 			// if (i - R0 > 0) goto END
	D=M
	@R0
	D=D-M
	@END
	D;JGT

	@i 			// i++
	M=M+1

	@R1 		// R2 += R1
	D=M
	@R2
	M=M+D

	@LOOP 		// goto LOOP
	0;JMP	

(END)
	@END
	0;JMP