#include "VmTranslator.h"
#include "VmCommandType.h"
#include "Parser.h"
#include "CodeWriter.h"
#include "CodeWriterOptimised.h"
#include <fstream>
#include <list>

VmTranslator::VmTranslator(string input, ostream& outputStream, bool verbose, bool bootstrap, bool comments, bool optimise)
	: input(input),
	  outputStream(outputStream),
	  fileHandler(input),
	  verbose(verbose),
	  bootstrap(bootstrap),
	  comments(comments),
	  optimise(optimise),
	  vmTransTitle("vm2hack"),
	  vmTransSubtitle("vm translator (nand2tetris, chap. 8)"),
	  vmTransVersion("0.1")
{
}

void VmTranslator::translate()
{
    if (verbose)
        printHeader();

    CodeWriter* codeWriter;

    if (!optimise)
        codeWriter = new CodeWriter(outputStream, fileHandler.getProgramName(), comments);
    else
        codeWriter = new CodeWriterOptimised(outputStream, fileHandler.getProgramName(), comments);

	if (bootstrap) {
        codeWriter->writeInit();

		if (verbose)
			cout << "bootstrap code written" << endl;
	}

	string file;
	int fileCount = 0;

	while (fileHandler.nextVmFile(file)) {

		if (verbose) {
			cout << "processing file " << FileHandler::removePath(file) << " -> module: ";
			cout << FileHandler::getProgramName(file) << endl;
		}

		ifstream inputFile(file);
		if (!inputFile.good()) {
			cerr << "error: unable to open file \"" << file << "\"" << endl;
			continue;
		}

		Parser parser(inputFile);
		codeWriter->setFileName(file);

        if (!optimise)
            translateNonOptimised(parser, codeWriter);
        else
            translateOptimised(parser, (CodeWriterOptimised*) codeWriter);

		fileCount++;

	}

    delete codeWriter;

    if (verbose) {
		cout << endl;
		cout << "Number of files translated: " << fileCount << endl;
		cout << "done" << endl << endl;
	}
}

void VmTranslator::translateNonOptimised(Parser& parser, CodeWriter* codeWriter)
{
    while (parser.advance()) {

        switch (parser.commandType()) {

            case VmCommandType::C_ARITHMETIC:
                codeWriter->writeArithmetic(parser.getCommand());
                break;

            case VmCommandType::C_PUSH:
            case VmCommandType::C_POP:
                codeWriter->writePushPop(parser.commandType(), parser.arg1(), parser.arg2());
                break;

            case VmCommandType::C_LABEL:
                codeWriter->writeLabel(parser.arg1());
                break;

            case VmCommandType::C_GOTO:
                codeWriter->writeGoto(parser.arg1());
                break;

            case VmCommandType::C_IF:
                codeWriter->writeIf(parser.arg1());
                break;

            case VmCommandType::C_FUNCTION:
                if (verbose)
                    cout << "  ~ function " << parser.arg1() << " (allocates " << parser.arg2() << " local vars)" << endl;

                codeWriter->writeFunction(parser.arg1(), parser.arg2());
                break;

            case VmCommandType::C_CALL:
                codeWriter->writeCall(parser.arg1(), parser.arg2());
                break;

            case VmCommandType::C_RETURN:
                codeWriter->writeReturn();
                break;

        }
    }
}

void VmTranslator::translateOptimised(Parser& parser, CodeWriterOptimised* codeWriterOpt)
{
    static bool globalFunctions = true;

    if (globalFunctions) {
        codeWriterOpt->writeGlobalFunctions();
        globalFunctions = false;
    }

    vmCmdList.clear();

    while (parser.advance())
        vmCmdList.push_back(parser.commandOptType());

    parser.reset();

    while (parser.advance()) {

        if (translateOptimisedSequence(parser, codeWriterOpt))
            continue;

        switch (parser.commandType()) {

            case VmCommandType::C_ARITHMETIC:
                codeWriterOpt->writeArithmetic(parser.getCommand());
                break;

            case VmCommandType::C_PUSH:
            case VmCommandType::C_POP:
                codeWriterOpt->writePushPop(parser.commandType(), parser.arg1(), parser.arg2());
                break;

            case VmCommandType::C_LABEL:
                codeWriterOpt->writeLabel(parser.arg1());
                break;

            case VmCommandType::C_GOTO:
                codeWriterOpt->writeGoto(parser.arg1());
                break;

            case VmCommandType::C_IF:
                codeWriterOpt->writeIf(parser.arg1());
                break;

            case VmCommandType::C_FUNCTION:
                if (verbose)
                    cout << "  ~ function " << parser.arg1() << " (allocates " << parser.arg2() << " local vars)" << endl;

                codeWriterOpt->writeFunction(parser.arg1(), parser.arg2());
                break;

            case VmCommandType::C_CALL:
                codeWriterOpt->writeCall(parser.arg1(), parser.arg2());
                break;

            case VmCommandType::C_RETURN:
                codeWriterOpt->writeReturn();
                break;

        }

        if (!vmCmdList.empty())
            vmCmdList.erase(vmCmdList.begin());

    }
}

bool VmTranslator::translateOptimisedSequence(Parser& parser, CodeWriterOptimised* codeWriterOpt)
{
    if (vmCmdList.size() >= 3) {
        if (vmCmdList.at(0) == VmCommandOptType::C_PUSH_CONSTANT &&
            vmCmdList.at(1) == VmCommandOptType::C_PUSH_LOCAL &&
            vmCmdList.at(2) == VmCommandOptType::C_ARITHMETIC_ADD)
            {
                int constant = parser.arg2();
                parser.advance();
                int index = parser.arg2();

                codeWriterOpt->writeSeqPushConstantPushTempAdd(constant, index);
                vmCmdList.erase(vmCmdList.begin(), vmCmdList.begin() + 3);
                parser.advance();
                return true;
            }
    }

    if (vmCmdList.size() >= 2) {

        if (vmCmdList.at(0) == VmCommandOptType::C_PUSH_CONSTANT &&
            vmCmdList.at(1) == VmCommandOptType::C_ARITHMETIC_ADD)
            {
                codeWriterOpt->writeSeqPushConstantAdd(parser.arg2());
                vmCmdList.erase(vmCmdList.begin(), vmCmdList.begin() + 2);
                parser.advance();
                return true;
            }

        if (vmCmdList.at(0) == VmCommandOptType::C_PUSH_CONSTANT &&
            vmCmdList.at(1) == VmCommandOptType::C_ARITHMETIC_SUB)
            {
                codeWriterOpt->writeSeqPushConstantSub(parser.arg2());
                vmCmdList.erase(vmCmdList.begin(), vmCmdList.begin() + 2);
                parser.advance();
                return true;
            }

        if (vmCmdList.at(0) == VmCommandOptType::C_PUSH_CONSTANT &&
            vmCmdList.at(1) == VmCommandOptType::C_ARITHMETIC_NEG)
            {
                codeWriterOpt->writeSeqPushConstantNeg(parser.arg2());
                vmCmdList.erase(vmCmdList.begin(), vmCmdList.begin() + 2);
                parser.advance();
                return true;
            }

        if ((vmCmdList.at(0) == VmCommandOptType::C_PUSH_ARGUMENT ||
             vmCmdList.at(0) == VmCommandOptType::C_PUSH_LOCAL ||
             vmCmdList.at(0) == VmCommandOptType::C_PUSH_THIS ||
             vmCmdList.at(0) == VmCommandOptType::C_PUSH_THAT) &&
             vmCmdList.at(1) == VmCommandOptType::C_ARITHMETIC_ADD)
            {
                codeWriterOpt->writeSeqPushArgLocalThisThatAdd(parser.arg1(), parser.arg2());
                vmCmdList.erase(vmCmdList.begin(), vmCmdList.begin() + 2);
                parser.advance();
                return true;
            }

        if ((vmCmdList.at(0) == VmCommandOptType::C_PUSH_ARGUMENT ||
             vmCmdList.at(0) == VmCommandOptType::C_PUSH_LOCAL ||
             vmCmdList.at(0) == VmCommandOptType::C_PUSH_THIS ||
             vmCmdList.at(0) == VmCommandOptType::C_PUSH_THAT) &&
             vmCmdList.at(1) == VmCommandOptType::C_ARITHMETIC_SUB)
            {
                codeWriterOpt->writeSeqPushArgLocalThisThatSub(parser.arg1(), parser.arg2());
                vmCmdList.erase(vmCmdList.begin(), vmCmdList.begin() + 2);
                parser.advance();
                return true;
            }

        if (vmCmdList.at(0) == VmCommandOptType::C_PUSH_TEMP &&
            vmCmdList.at(1) == VmCommandOptType::C_POP_THAT)
            {
                int tempIndex = parser.arg2();
                parser.advance();
                int thatIndex = parser.arg2();

                codeWriterOpt->writeSeqPushTempPopThat(tempIndex, thatIndex);
                vmCmdList.erase(vmCmdList.begin(), vmCmdList.begin() + 2);
                return true;
            }

        if (vmCmdList.at(0) == VmCommandOptType::C_PUSH_ARGUMENT &&
            vmCmdList.at(1) == VmCommandOptType::C_POP_POINTER)
            {
                int argIndex = parser.arg2();
                parser.advance();
                int pointerIndex = parser.arg2();

                cout << "HERE" << endl;

                codeWriterOpt->writeSeqPushArgumentPopPointer(argIndex, pointerIndex);
                vmCmdList.erase(vmCmdList.begin(), vmCmdList.begin() + 2);
                return true;
            }

        if (vmCmdList.at(0) == VmCommandOptType::C_PUSH_STATIC &&
            vmCmdList.at(1) == VmCommandOptType::C_ARITHMETIC_ADD)
            {
                codeWriterOpt->writeSeqPushStaticAdd(parser.arg2());
                vmCmdList.erase(vmCmdList.begin(), vmCmdList.begin() + 2);
                parser.advance();
                return true;
            }

    }

    return false;
}



























/*
void VmTranslator::translateOptimised(Parser& parser, CodeWriterOptimised* codeWriterOpt)
{
    static bool globalFunctions = true;

    if (globalFunctions)
        codeWriterOpt->writeGlobalFunctions();

    while (!vmCommands.empty() || parser.advance()) {

        switch (parser.commandType()) {

            case VmCommandType::C_ARITHMETIC:
                codeWriterOpt->writeArithmetic(parser.getCommand());
                break;

            case VmCommandType::C_PUSH:
            case VmCommandType::C_POP:
                codeWriterOpt->writePushPop(parser.commandType(), parser.arg1(), parser.arg2());
                break;

            case VmCommandType::C_LABEL:
                codeWriterOpt->writeLabel(parser.arg1());
                break;

            case VmCommandType::C_GOTO:
                codeWriterOpt->writeGoto(parser.arg1());
                break;

            case VmCommandType::C_IF:
                codeWriterOpt->writeIf(parser.arg1());
                break;

            case VmCommandType::C_FUNCTION:
                if (verbose)
                    cout << "  ~ function " << parser.arg1() << " (allocates " << parser.arg2() << " local vars)" << endl;

                codeWriterOpt->writeFunction(parser.arg1(), parser.arg2());
                break;

            case VmCommandType::C_CALL:
                codeWriterOpt->writeCall(parser.arg1(), parser.arg2());
                break;

            case VmCommandType::C_RETURN:
                codeWriterOpt->writeReturn();
                break;

        }
    }
}
*/

void VmTranslator::printHeader()
{
	cout << vmTransTitle << " - " << vmTransSubtitle << " v" << vmTransVersion << endl;
	cout << "input " << input;

	if (fileHandler.isDir()) {
		cout << " (directory with " << fileHandler.getNumVmFiles();
		fileHandler.getNumVmFiles() == 1 ? cout << " vm file)" : cout << " vm files)";
	}

	cout << endl;
	cout << "output to file " << fileHandler.getOutputFile() << endl << endl;
}
