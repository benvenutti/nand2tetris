#include "CodeWriterOptimised.h"

CodeWriterOptimised::CodeWriterOptimised(ostream& outputStream, string programName, bool writeComments)
	: CodeWriter(outputStream, programName, writeComments),
	  eqGlobalID(0),
	  gtGlobalID(0),
	  ltGlobalID(0)
{
}

void CodeWriterOptimised::writePushConstant(int index)
{
	string indexStr = to_string(index);
	string com("// push constant " + indexStr);

	if (index == 0 || index == 1 || index == -1) {

		write("@SP", com);
		write("A=M");
		write("M=" + indexStr);
		write("@SP");
		write("M=M+1");

	} else if (index == 2) {

		write("@SP");
		write("A=M");
		write("M=1");
		write("M=M+1");
		write("@SP");
		write("M=M+1");

	} else {

        write("@" + indexStr, com);
        write("D=A");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

	}
}

void CodeWriterOptimised::writePush(string segment, int index)
{
    string indexStr = to_string(index);
    string registerStr = registerName(segment, index);
    string com("// push " + segment + " " + indexStr);

    if (segment == "constant") { // push constant

    	writePushConstant(index);

    } else if (segment == "static" || segment == "temp" || segment == "pointer") { // push static, temp or pointer

        write("@" + registerStr, com);
        write("D=M");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

    } else { // all other segments

    	if (index == 0) {

    		write("@" + registerStr, com);
	        write("A=M");
	        write("D=M");
	        write("@SP");
	        write("A=M");
	        write("M=D");
	        write("@SP");
	        write("M=M+1");

    	} else if (index == 1) {

    		write("@" + registerStr, com);
	        write("A=M+1");
	        write("D=M");
	        write("@SP");
	        write("A=M");
	        write("M=D");
	        write("@SP");
	        write("M=M+1");

	    } else if (index == 2) {

	    	write("@" + registerStr, com);
	        write("A=M+1");
	        write("A=A+1");
	        write("D=M");
	        write("@SP");
	        write("A=M");
	        write("M=D");
	        write("@SP");
	        write("M=M+1");

    	} else {

	        write("@" + registerStr, com);
	        write("D=M");
	        write("@" + indexStr);
	        write("A=D+A");
	        write("D=M");
	        write("@SP");
	        write("A=M");
	        write("M=D");
	        write("@SP");
	        write("M=M+1");

    	}

    }
}

void CodeWriterOptimised::writeInit()
{
    /*
        SP=256
        call Sys.init
    */

    string com("// bootstrap code");

    // this label is for a guard loop in case Sys.init returns (it is not supposed to)
    string bootstrapLabel("BOOTSTRAP_" + programName + "$GUARD_LOOP");

    write("@256", com);
    write("D=A");
    write("@SP");
    write("M=D");
    write("@Sys.init");
    write("D=A");
    write("@R13"); // R13 = functionName
    write("M=D");
    write("D=0");
    write("@R14"); // R14 = numArgs
    write("M=D");
    writeCall("Sys.init", 0);
    write("(" + bootstrapLabel + ")", false);
    write("@" + bootstrapLabel);
    write("0;JMP");
}

void CodeWriterOptimised::writeCall(string functionName, int numArgs)
{
    /*
        push return-address     // (using the label declared below)
        push LCL                // save LCL of the calling function
        push ARG                // save ARG of the calling function
        push THIS               // save THIS of the calling function
        push THAT               // save THAT of the calling function
        ARG = SP-n-5            // reposition ARG (n = number of args)
        LCL = SP                // reposiiton LCL
        goto f                  // transfer control
        (return-address)        // declare a label for the return-address
    */

    static bool firstCall = true;

    string returnSymbol(functionName + "_RETURN_" + to_string(callGlobalID));
    callGlobalID++;

    if (firstCall) {

    	firstCall = false;
    	string com("// global call");

	    write("@" + returnSymbol, com); // push return-addr
	    write("D=A");
	    write("(GLOBAL_CALL)", false);
	    write("@SP");
	    write("A=M");
	    write("M=D");
	    write("@SP");
	    write("M=M+1");

	    write("@LCL"); // push LCL
	    write("D=M");
	    write("@SP");
	    write("A=M");
	    write("M=D");
	    write("@SP");
	    write("M=M+1");

	    write("@ARG"); // push ARG
	    write("D=M");
	    write("@SP");
	    write("A=M");
	    write("M=D");
	    write("@SP");
	    write("M=M+1");

	    write("@THIS"); // push THIS
	    write("D=M");
	    write("@SP");
	    write("A=M");
	    write("M=D");
	    write("@SP");
	    write("M=M+1");

	    write("@THAT"); // push THAT
	    write("D=M");
	    write("@SP");
	    write("A=M");
	    write("M=D");
	    write("@SP");
	    write("M=M+1");

	    write("@R14"); // ARG (R14) = SP-n-5
	    write("D=M");
	    write("@5");
	    write("D=D+A");
	    write("@SP");
	    write("D=M-D");
	    write("@ARG");
	    write("M=D");

	    write("@SP"); // LCL = SP
	    write("D=M");
	    write("@LCL");
	    write("M=D");

	    write("@R13"); // goto f (R13)
	    write("A=M");
	    write("0;JMP");

	    write("(" + returnSymbol + ")", false); // (return-address)

	} else {

		string com("// call " + functionName + " " + to_string(numArgs));

		write("@" + functionName, com); // R13 = functionName
		write("D=A");
		write("@R13");
		write("M=D");

		write("@" + to_string(numArgs)); // R14 = numArgs
		write("D=A");
		write("@R14");
		write("M=D");

		write("@" + returnSymbol);
		write("D=A");
		write("@GLOBAL_CALL");
		write("0;JMP");
		write("(" + returnSymbol + ")", false); // (return-address)

	}
}

void CodeWriterOptimised::writeReturn()
{
    /*
        FRAME = LCL         // FRAME is a temporary var
        RET = *(FRAME-5)    // put the return-address in a temporary var
        *ARG = pop()        // reposition the return value for the caller
        SP = ARG + 1        // restore SP of the caller
        THAT = *(FRAME - 1) // restore THAT of the caller
        THIS = *(FRAME - 2) // restore THIS of the caller
        ARG = *(FRAME - 3)  // restore ARG of the caller
        LCL = *(FRAME - 4)  // restore LCL of the caller
        goto RET            // goto return-address (in the caller's code)
    */

    static bool firstReturn = true;

    if (firstReturn) {

    	firstReturn = false;
	    string com("// global return");

	    write("(GLOBAL_RETURN)", com, false);
	    write("@LCL", com); // FRAME = LCL
	    write("D=M");
	    write("@R13"); // R13 -> FRAME
	    write("M=D");

	    write("@5"); // RET = *(FRAME-5)
	    write("A=D-A");
	    write("D=M");
	    write("@R14"); // R14 -> RET
	    write("M=D");

	    write("@SP"); // *ARG = pop()
	    write("AM=M-1");
	    write("D=M");
	    write("@ARG");
	    write("A=M");
	    write("M=D");

	    write("D=A"); // SP = ARG+1
	    write("@SP");
	    write("M=D+1");

	    write("@R13"); // THAT = *(FRAME-1)
	    write("AM=M-1");
	    write("D=M");
	    write("@THAT");
	    write("M=D");

	    write("@R13"); // THIS = *(FRAME-2)
	    write("AM=M-1");
	    write("D=M");
	    write("@THIS");
	    write("M=D");

	    write("@R13"); // ARG = *(FRAME-3)
	    write("AM=M-1");
	    write("D=M");
	    write("@ARG");
	    write("M=D");

	    write("@R13"); // LCL = *(FRAME-4)
	    write("AM=M-1");
	    write("D=M");
	    write("@LCL");
	    write("M=D");

	    write("@R14"); // goto RET
	    write("A=M");
	    write("0;JMP");

	} else {

		string com("// return");

		write("@GLOBAL_RETURN", com);
		write("0;JMP");

	}
}

void CodeWriterOptimised::writeFunction(string functionName, int numLocals)
{
    /*
        (f)             // declare a label for the function entry
        repeat k times: // k = number of local vars
        PUSH 0
    */

    this->functionName = functionName;
    string com("// function " + functionName + " " + to_string(numLocals));

    write("(" + functionName + ")", com, false);

    if (numLocals == 0)
        return;

    if (numLocals == 1) {
        write("@0");
        write("D=A");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

        return;
    }

    if (numLocals <= 5) {
        write("@0");
        write("D=A");
        write("@SP");
        write("A=M");
        write("M=D");

        for (int i = 1; i < numLocals; i++) {
            write("A=A+1");
            write("M=D");
        }

        write("D=A+1");
        write("@SP");
        write("M=D");

        return;
    }

    string loopLabel(functionName + "_INIT_LOCALS_LOOP");
    string loopEndLabel(functionName + "_INIT_LOCALS_END");

    write("@" + to_string(numLocals));
    write("D=A");
    write("@R13"); // temp
    write("M=D");
    write("(" + loopLabel + ")", false);
    write("@" + loopEndLabel);
    write("D;JEQ");
    write("@0");
    write("D=A");
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");
    write("@R13");
    write("MD=M-1");
    write("@" + loopLabel);
    write("0;JMP");
    write("(" + loopEndLabel + ")", false);
}

void CodeWriterOptimised::writeGlobalFunctions()
{
	string com("// global functions");

    write("@SKIP_GLOBAL_FUNCTIONS", com); // skip global functions in case there's no bootstrap
    write("0;JMP");

    write("(GLOBAL_EQ)", false);
    write("@R15"); // save return address in R15 (ret addr is in D)
    write("M=D");
    write("@SP");
    write("AM=M-1");
    write("D=M");
    write("@SP");
    write("AM=M-1");
    write("D=M-D");
    write("@JUMP_EQ");
    write("D;JEQ");
    write("D=1");
    write("(JUMP_EQ)", false);
    write("D=D-1");
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");
    write("@R15"); // return address
    write("A=M");
    write("0;JMP");

    write("(GLOBAL_GT)", false);
    write("@R15"); // save return address in R15 (ret addr is in D)
    write("M=D");
    write("@SP");
    write("AM=M-1");
    write("D=M");
    write("@SP");
    write("AM=M-1");
    write("D=M-D");
    write("@JUMP_GT");
    write("D;JGT");
    write("D=0");
    write("@JUMP_GT_END");
    write("0; JMP");
    write("(JUMP_GT)", false);
    write("D=-1");
    write("(JUMP_GT_END)", false);
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");
    write("@R15"); // return address
    write("A=M");
    write("0;JMP");

    write("(GLOBAL_LT)", false);
    write("@R15"); // save return address in R15 (ret addr is in D)
    write("M=D");
    write("@SP");
    write("AM=M-1");
    write("D=M");
    write("@SP");
    write("AM=M-1");
    write("D=M-D");
    write("@JUMP_LT");
    write("D;JLT");
    write("D=0");
    write("@JUMP_LT_END");
    write("0; JMP");
    write("(JUMP_LT)", false);
    write("D=-1");
    write("(JUMP_LT_END)", false);
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");
    write("@R15"); // return address
    write("A=M");
    write("0;JMP");

    write("(SKIP_GLOBAL_FUNCTIONS)", false);
}

void CodeWriterOptimised::writeArithmeticEq()
{
    string com("// eq");
    string returnLabel("GLOBAL_EQ_RETURN_" + to_string(eqGlobalID));
    eqGlobalID++;

    write("@" + returnLabel, com);
    write("D=A"); // (return address is in D)
    write("@GLOBAL_EQ");
    write("0;JMP");
    write("(" + returnLabel + ")", false);
}

void CodeWriterOptimised::writeArithmeticGt()
{
    string com("// gt");
    string returnLabel("GLOBAL_GT_RETURN_" + to_string(eqGlobalID));
    eqGlobalID++;

    write("@" + returnLabel, com);
    write("D=A"); // (return address is in D)
    write("@GLOBAL_GT");
    write("0;JMP");
    write("(" + returnLabel + ")", false);
}

void CodeWriterOptimised::writeArithmeticLt()
{
    string com("// lt");
    string returnLabel("GLOBAL_LT_RETURN_" + to_string(eqGlobalID));
    eqGlobalID++;

    write("@" + returnLabel, com);
    write("D=A"); // (return address is in D)
    write("@GLOBAL_LT");
    write("0;JMP");
    write("(" + returnLabel + ")", false);
}

void CodeWriterOptimised::writeSeqPushConstantAdd(int index)
{
    string com("// seq 2: push constant " + to_string(index) + " + add");

    if (index == 1) {

        write("@SP", com);
        write("A=M-1");
        write("D=M+1");
        write("M=D");

    } else {

        write("@SP", com);
        write("A=M-1");
        write("D=M");
        write("@" + to_string(index));
        write("D=D+A");
        write("@SP");
        write("A=M-1");
        write("M=D");

    }
}

void CodeWriterOptimised::writeSeqPushConstantSub(int index)
{
    string com("// seq 2: push constant " + to_string(index) + " + sub");

    if (index == 1) {

        write("@SP", com);
        write("A=M-1");
        write("D=M-1");
        write("M=D");

    } else {

        write("@SP", com);
        write("A=M-1");
        write("D=M");
        write("@" + to_string(index));
        write("D=D-A");
        write("@SP");
        write("A=M-1");
        write("M=D");

    }
}

void CodeWriterOptimised::writeSeqPushConstantNeg(int index)
{
    string com("// seq 2: push constant " + to_string(index) + " + neg");

    if (index == 1) {

        write("@SP", com);
        write("A=M");
        write("M=-1");
        write("@SP");
        write("M=M+1");

    } else {

        write("@" + to_string(index));
        write("D=-A");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

    }

}

void CodeWriterOptimised::writeSeqPushArgLocalThisThatAdd(string segment, int index)
{
    string com("// seq 2: push " + segment + " " + to_string(index) + " + add");
    string registerStr = registerName(segment, index);

    if (index == 0) {

        write("@" + registerStr, com);
        write("A=M");
        write("D=M");
        write("@SP");
        write("A=M-1");
        write("M=M+D");

    } else if (index == 1) {

        write("@" + registerStr, com);
        write("A=M+1");
        write("D=M");
        write("@SP");
        write("A=M-1");
        write("M=M+D");

    } else if (index == 2) {

        write("@" + registerStr, com);
        write("A=M+1");
        write("A=A+1");
        write("D=M");
        write("@SP");
        write("A=M-1");
        write("M=M+D");

    } else {

        write("@" + registerStr, com);
        write("D=M");
        write("@" + to_string(index));
        write("A=D+A");
        write("D=M");
        write("@SP");
        write("A=M-1");
        write("M=M+D");

    }
}

void CodeWriterOptimised::writeSeqPushArgLocalThisThatSub(string segment, int index)
{
    string com("// seq 2: push " + segment + " " + to_string(index) + " + sub");
    string registerStr = registerName(segment, index);

    if (index == 0) {

        write("@" + registerStr, com);
        write("A=M");
        write("D=M");
        write("@SP");
        write("A=M-1");
        write("M=M-D");

    } else if (index == 1) {

        write("@" + registerStr, com);
        write("A=M+1");
        write("D=M");
        write("@SP");
        write("A=M-1");
        write("M=M-D");

    } else if (index == 2) {

        write("@" + registerStr, com);
        write("A=M+1");
        write("A=A+1");
        write("D=M");
        write("@SP");
        write("A=M-1");
        write("M=M-D");

    } else {

        write("@" + registerStr, com);
        write("D=M");
        write("@" + to_string(index));
        write("A=D+A");
        write("D=M");
        write("@SP");
        write("A=M-1");
        write("M=M-D");

    }
}

void CodeWriterOptimised::writeSeqPushTempPopThat(int tempIndex, int thatIndex)
{
    string com("// seq 2: push temp " + to_string(tempIndex) + " + pop that " + to_string(thatIndex));
    string registerTempStr = registerName("temp", tempIndex);

    if (thatIndex == 0) {

        write("@" + registerTempStr, com);
        write("D=M");
        write("@THAT");
        write("A=M");
        write("M=D");

    } else if (thatIndex == 1) {

        write("@" + registerTempStr, com);
        write("D=M");
        write("@THAT");
        write("A=M+1");
        write("M=D");

    } else {

        write("@THAT", com);
        write("D=M");
        write("@" + to_string(thatIndex));
        write("D=D+A");
        write("@SP");
        write("M=D");
        write("@" + registerTempStr, com);
        write("D=M");
        write("@SP");
        write("A=M");
        write("M=D");

    }
}

void CodeWriterOptimised::writeSeqPushStaticAdd(int index)
{
    string com("// seq 2: push static " + to_string(index) + " + add");
    string registerStr = registerName("static", index);

    write("@" + registerStr, com);
    write("D=M");
    write("@SP");
    write("A=M-1");
    write("M=D+M");

}

void CodeWriterOptimised::writeSeqPushArgumentPopPointer(int argIndex, int pointerIndex)
{
    string com("// seq 2: push argument " + to_string(argIndex) + " + pop pointer " + to_string(pointerIndex));

    if (argIndex == 0 && pointerIndex == 0) {

        write("@ARG", com);
        write("A=M");
        write("D=M");
        write("@R3");
        write("M=D");

    } else if (argIndex == 0 && pointerIndex == 1) {

        write("@ARG", com);
        write("A=M");
        write("D=M");
        write("@R4");
        write("M=D");

    } else if (argIndex == 1 && pointerIndex == 0) {

        write("@ARG", com);
        write("A=M+1");
        write("D=M");
        write("@R3");
        write("M=D");

    } else if (argIndex == 1 && pointerIndex == 1) {

        write("@ARG", com);
        write("A=M+1");
        write("D=M");
        write("@R4");
        write("M=D");

    } else {

        if (pointerIndex == 0) {

            write("@ARG", com);
            write("D=M");
            write("@" + to_string(argIndex));
            write("A=D+A");
            write("D=M");
            write("@R3");
            write("M=D");

        } else {

            write("@ARG", com);
            write("D=M");
            write("@" + to_string(argIndex));
            write("A=D+A");
            write("D=M");
            write("@R4");
            write("M=D");

        }

    }
}

void CodeWriterOptimised::writeSeqPushConstantPushTempAdd(int constant, int index)
{
    string com("// seq 3: push constant " + to_string(constant) + " + push local " + to_string(index) + " + add");
    string registerStr = registerName("local", index);

    if (index == 0) {

        write("@" + registerStr, com);
        write("A=M");
        write("D=M");
        write("@" + to_string(constant));
        write("D=D+A");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

    } else if (index == 1) { // push constant 1

        write("@" + registerStr, com);
        write("A=M+1");
        write("D=M");
        write("@" + to_string(constant));
        write("D=D+A");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

    } else {

        write("@" + registerStr, com);
        write("D=M");
        write("@" + to_string(index));
        write("A=A+D");
        write("D=M");
        write("@" + to_string(constant));
        write("D=D+A");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

    }
}










