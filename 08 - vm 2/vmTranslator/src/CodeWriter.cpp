#include "CodeWriter.h"
#include "FileHandler.h"

CodeWriter::CodeWriter(ostream& outputStream, string programName, bool writeComments)
	: outputStream(outputStream), 
      programName(programName), 
      writeComments(writeComments),
      callGlobalID(0), 
      eqModuleID(0),
      gtModuleID(0),
      ltModuleID(0)
{
    if (writeComments)
        writeProgramHeader();
}

void CodeWriter::setFileName(string fileName)
{
    this->fileName = FileHandler::removePath(fileName);
    moduleName = FileHandler::getProgramName(fileName); // removes path and extension of file
	
    // reset module IDs since a new file/module is up for translation
    eqModuleID = 0; 
    gtModuleID = 0;
    ltModuleID = 0;

    if (writeComments)
	   writeModuleHeader();
}

void CodeWriter::writeArithmetic(string command)
{
    if (command == "add")      writeArithmeticAdd();
    else if (command == "sub") writeArithmeticSub();
    else if (command == "neg") writeArithmeticNeg();
    else if (command == "eq")  writeArithmeticEq();
    else if (command == "gt")  writeArithmeticGt();
    else if (command == "lt")  writeArithmeticLt();
    else if (command == "and") writeArithmeticAnd();
    else if (command == "or")  writeArithmeticOr();
    else writeArithmeticNot();
}

void CodeWriter::writeArithmeticAdd()
{
    string com("// add"); // "com" for comment

    write("@SP", com);
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=D+M");
}

void CodeWriter::writeArithmeticSub()
{
    string com("// sub");

    write("@SP", com);
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=M-D");
}

void CodeWriter::writeArithmeticNeg()
{
    string com("// neg");

    write("@SP", com);
    write("A=M");
    write("A=A-1");
    write("M=-M");
}

void CodeWriter::writeArithmeticEq()
{
    string com("// eq");
    string label("JEQ_" + moduleName + "_" + to_string(eqModuleID));
    eqModuleID++;

    write("@SP", com);
    write("AM=M-1");
    write("D=M");
    write("@SP");
    write("AM=M-1");
    write("D=M-D");
    write("@" + label);
    write("D;JEQ");
    write("D=1");
    write("(" + label + ")", false);
    write("D=D-1");
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");
}

void CodeWriter::writeArithmeticGt()
{
    string com("// gt");
    string labelTrue("JGT_TRUE_" + moduleName + "_" + to_string(gtModuleID));
    string labelFalse("JGT_FALSE_" + moduleName + "_" + to_string(gtModuleID));
    gtModuleID++;

    write("@SP", com);
    write("AM=M-1");
    write("D=M");
    write("@SP");
    write("AM=M-1");
    write("D=M-D");
    write("@" + labelTrue);
    write("D;JGT");
    write("D=0");
    write("@" + labelFalse);
    write("0;JMP");
    write("(" + labelTrue + ")", false);
    write("D=-1");
    write("(" + labelFalse + ")", false);
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");
}

void CodeWriter::writeArithmeticLt()
{
    string com("// lt");
    string labelTrue("JLT_TRUE_" + moduleName + "_" + to_string(ltModuleID));
    string labelFalse("JLT_FALSE_" + moduleName + "_" + to_string(ltModuleID));
    ltModuleID++;

    write("@SP", com);
    write("AM=M-1");
    write("D=M");
    write("@SP");
    write("AM=M-1");
    write("D=M-D");
    write("@" + labelTrue);
    write("D;JLT");
    write("D=0");
    write("@" + labelFalse);
    write("0;JMP");
    write("(" + labelTrue + ")", false);
    write("D=-1");
    write("(" + labelFalse + ")", false);
    write("@SP");
    write("A=M");
    write("M=D");
    write("@SP");
    write("M=M+1");
}

void CodeWriter::writeArithmeticAnd()
{
    string com("// and");

    write("@SP", com);
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=D&M");
}

void CodeWriter::writeArithmeticOr()
{
    string com("// or");

    write("@SP", com);
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=D|M");
}

void CodeWriter::writeArithmeticNot()
{
    string com("// not");

    write("@SP", com);
    write("A=M");
    write("A=A-1");
    write("M=!M");
}

void CodeWriter::writePushPop(VmCommandType cmd, string segment, int index)
{
    if (cmd == VmCommandType::C_PUSH)
        writePush(segment, index);
    else
        writePop(segment, index);
}

void CodeWriter::writePush(string segment, int index)
{
    string indexStr = to_string(index);
    string registerStr = registerName(segment, index);
    string com("// push " + segment + " " + indexStr);

    if (segment == "constant") { // push constant

        write("@" + indexStr, com);
        write("D=A");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

    } else if (segment == "static" || segment == "temp" || segment == "pointer") { // push static, temp or pointer

        write("@" + registerStr, com);
        write("D=M");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

    } else { // all other segments

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

void CodeWriter::writePop(string segment, int index)
{
    string indexStr = to_string(index);
    string registerStr = registerName(segment, index);
    string com("// pop " + segment + " " + indexStr);

    // pop static, temp or pointer
    if (segment == "static" || segment == "temp" || segment == "pointer") {

        write("@SP", com);
        write("AM=M-1");
        write("D=M");
        write("@" + registerStr);
        write("M=D");

    } else { // all other segments

        write("@" + registerStr, com);
        write("D=M");
        write("@" + indexStr);
        write("D=D+A");
        write("@R13");
        write("M=D");
        write("@SP");
        write("AM=M-1");
        write("D=M");
        write("@R13");
        write("A=M");
        write("M=D");

    }
}

void CodeWriter::writeInit()
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
    writeCall("Sys.init", 0);
    write("(" + bootstrapLabel + ")", false);
    write("@" + bootstrapLabel);
    write("0;JMP");
}

void CodeWriter::writeLabel(string label)
{
    string com("// label " + label);

    write("(" + labelSymbol(label) + ")", com ,false);
}

void CodeWriter::writeGoto(string label)
{
    string com("// goto " + label);

    write("@" + labelSymbol(label), com);
    write("0;JMP");
}

void CodeWriter::writeIf(string label)
{
    string com("// if-goto " + label);

    write("@SP", com);
    write("AM=M-1");
    write("D=M");
    write("@" + labelSymbol(label));
    write("D;JNE");
}

void CodeWriter::writeCall(string functionName, int numArgs)
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

    string com("// call " + functionName + " " + to_string(numArgs));
    string returnSymbol(functionName + "_RETURN_" + to_string(callGlobalID));
    callGlobalID++;

    write("@" + returnSymbol, com); // push return-addr
    write("D=A");
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

    write("@" + to_string(numArgs)); // ARG = SP-n-5
    write("D=A");
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

    write("@" + functionName); // goto f
    write("0;JMP");

    write("(" + returnSymbol + ")", false); // (return-address)
}

void CodeWriter::writeReturn()
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

    string com("// return");

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
}

void CodeWriter::writeFunction(string functionName, int numLocals)
{
    /*
        (f)             // declare a label for the function entry
        repeat k times: // k = number of local vars
        PUSH 0
    */

    this->functionName = functionName;
    string loopLabel(functionName + "_INIT_LOCALS_LOOP");
    string loopEndLabel(functionName + "_INIT_LOCALS_END");
    string com("// function " + functionName + " " + to_string(numLocals));

    write("(" + functionName + ")", com, false);
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

void CodeWriter::writeProgramHeader()
{
	outputStream << "// ............................................" << endl;
    outputStream << "// program: " << programName << ".asm" << endl;
    outputStream << "// ............................................" << endl;
    outputStream << endl;
}

void CodeWriter::writeModuleHeader()
{
	outputStream << endl;
    outputStream << "// ............................................" << endl;
    outputStream << "// module: " << moduleName << endl;
    outputStream << "// from file: " << fileName << endl;
    outputStream << "// ............................................" << endl;
 	outputStream << endl;
}

void CodeWriter::write(string code, bool ident)
{
    if (ident)
        outputStream << "\t";

    outputStream << code << endl;
}

void CodeWriter::write(string code, string comment, bool ident)
{
    if (ident)
        outputStream << "\t";

    outputStream << code;

    if (writeComments)
        outputStream << "\t\t" << comment;

    outputStream << endl;
}

string CodeWriter::labelSymbol(string label)
{
    if (functionName.empty())
        // there should not be any label/goto statement before the 1st function, but this is just in case...
        return programName + ".asm$" + label;
    else
        return functionName + "$" + label;
}

string CodeWriter::registerName(string segment,int index)
{
    if (segment == "local")    return "LCL";
    if (segment == "argument") return "ARG";
    if (segment == "this")     return "THIS";
    if (segment == "that")     return "THAT";
    if (segment == "pointer")  return "R" + to_string(3 + index);
    if (segment == "temp")     return "R" + to_string(5 + index);

    return moduleName + "." + to_string(index); // else it is static
}