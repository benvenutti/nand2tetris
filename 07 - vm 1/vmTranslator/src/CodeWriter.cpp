#include "CodeWriter.h"
#include "FileHandler.h"

CodeWriter::CodeWriter(ostream& outputStream, string programName)
	: outputStream(outputStream), programName(programName), symbolCounter(0)
{
    writeProgramHeader();
}

void CodeWriter::setFileName(string fileName)
{
    this->fileName = FileHandler::removePath(fileName);
    moduleName = FileHandler::getProgramName(fileName);
	symbolCounter = 0;
	writeModuleHeader();
}

void CodeWriter::writeArithmetic(string command)
{
    if (command == "add") writeArithmeticAdd();
    else if (command == "sub") writeArithmeticSub();
    else if (command == "neg") writeArithmeticNeg();
    else if (command == "eq") writeArithmeticEq();
    else if (command == "gt") writeArithmeticGt();
    else if (command == "lt") writeArithmeticLt();
    else if (command == "and") writeArithmeticAnd();
    else if (command == "or") writeArithmeticOr();
    else writeArithmeticNot();
}

void CodeWriter::writeArithmeticAdd()
{
    write("@SP // add");
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=D+M");
}

void CodeWriter::writeArithmeticSub()
{
    write("@SP // sub");
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=M-D");
}

void CodeWriter::writeArithmeticNeg()
{
    write("@SP // neg");
    write("A=M");
    write("A=A-1");
    write("M=-M");
}

void CodeWriter::writeArithmeticEq()
{
    string label("JEQ_" + moduleName + "_" + to_string(symbolCounter));

    write("@SP // eq");
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

    symbolCounter++;
}

void CodeWriter::writeArithmeticGt()
{
    string labelTrue("JGT_TRUE_" + moduleName + "_" + to_string(symbolCounter));
    string labelFalse("JGT_FALSE_" + moduleName + "_" + to_string(symbolCounter));

    write("@SP // gt");
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

    symbolCounter++;
}

void CodeWriter::writeArithmeticLt()
{
    string labelTrue("JLT_TRUE_" + moduleName + "_" + to_string(symbolCounter));
    string labelFalse("JLT_FALSE_" + moduleName + "_" + to_string(symbolCounter));

    write("@SP // lt");
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

    symbolCounter++;
}

void CodeWriter::writeArithmeticAnd()
{
    write("@SP // and");
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=D&M");
}

void CodeWriter::writeArithmeticOr()
{
    write("@SP // or");
    write("AM=M-1");
    write("D=M");
    write("A=A-1");
    write("M=D|M");
}

void CodeWriter::writeArithmeticNot()
{
    write("@SP // not");
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

    if (segment == "constant") { // push constant

        write("@" + indexStr + " // push " + segment + " " + indexStr);
        write("D=A");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

    } else if (segment == "static" || segment == "temp" || segment == "pointer") { // push static or temp

        write("@" + registerStr + " // push " + segment + " " + indexStr);
        write("D=M");
        write("@SP");
        write("A=M");
        write("M=D");
        write("@SP");
        write("M=M+1");

    } else { // all other segments

        write("@" + registerStr + " // push " + segment + " " + indexStr);
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

    // pop static, temp or pointer
    if (segment == "static" || segment == "temp" || segment == "pointer") {

        write("@SP // pop " + segment + " " + indexStr);
        write("AM=M-1");
        write("D=M");
        write("@" + registerStr);
        write("M=D");

    } else { // all other segments

        write("@" + registerStr + " // pop " + segment + " " + indexStr);
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

void CodeWriter::write(string code, bool ident)
{
    if (ident)
        outputStream << "\t";
    outputStream << code << endl;
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

void CodeWriter::writeProgramHeader()
{
	outputStream << "// ............................................" << endl;
    outputStream << "// program: " << programName << ".asm" << endl;
    outputStream << "// ............................................" << endl;
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
