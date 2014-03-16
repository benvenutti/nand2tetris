#include "VmTranslator.h"
#include <fstream>

VmTranslator::VmTranslator(string input, ostream& outputStream, bool verbose, bool bootstrap, bool comments, bool optimise, bool unusedFunctions)
	: input(input),
	  outputStream(outputStream),
	  fileHandler(input),
	  verbose(verbose),
	  bootstrap(bootstrap),
	  comments(comments),
	  optimise(optimise),
	  unusedFunctions(unusedFunctions),
	  vmTransTitle("vm2asm"),
	  vmTransSubtitle("vm translator (nand2tetris, chap. 8)"),
	  vmTransVersion("0.2")
{
}

void VmTranslator::translate()
{
    if (verbose)
        printHeader();

    if (unusedFunctions) {
        scanFunctionCalls("Main", "Main.main");

        if (bootstrap) {
            funcCallMap.at("Sys").insert("Sys.init"); // MAY NOT HAVE "Sys"
            scanFunctionCalls("Sys", "Sys.halt");
        }
    }

    int fileCount;

    if (optimise)
        fileCount = translateOptimised();
    else
        fileCount = translateNonOptimised();

    if (verbose) {
        cout << endl;
        cout << "Number of files translated: " << fileCount << endl;
        cout << "done" << endl << endl;
    }
}

int VmTranslator::numSkippedFunctions()
{
    return skippedFuncVec.size();
}

void VmTranslator::printSkippedFunctions()
{
    for (auto& it: skippedFuncVec)
        cout << "- " << it << endl;
}

void VmTranslator::translateVmCommand(Parser& parser, CodeWriter* codeWriter)
{
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

int VmTranslator::translateNonOptimised()
{
    CodeWriter* codeWriter = new CodeWriter(outputStream, fileHandler.getProgramName(), comments);

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

        while (parser.advance()) {

            if (parser.commandType() == VmCommandType::C_FUNCTION) {

                string arg1 = parser.arg1();
                string moduleName = arg1.substr(0, arg1.find(".")); // ???????????? FUNC

                if (funcCallMap.find(moduleName) != funcCallMap.end()) {

                    if (funcCallMap.at(moduleName).find(arg1) == funcCallMap.at(moduleName).end()) {

                        skippedFuncVec.push_back(arg1);

                        while (parser.advance() && parser.commandType() != VmCommandType::C_RETURN);

                        continue;
                    }
                }
            }

            translateVmCommand(parser, codeWriter);
        }

        fileCount++;

    }

    delete codeWriter;

    return fileCount;
}

bool VmTranslator::advanceParser(Parser& parser, int num)
{
    for (int i = 0; i < num; i++) {
        bool ret = parser.advance();

        if (!ret)
            return false;
    }

    if (newFileHasStarted) {
        num--;
        newFileHasStarted = false;
    }

    vmCmdVec.erase(vmCmdVec.begin(), vmCmdVec.begin() + num);
    vmArg2Vec.erase(vmArg2Vec.begin(), vmArg2Vec.begin() + num);

    return true;
}

int VmTranslator::translateOptimised()
{
    CodeWriterOpt * codeWriterOpt = new CodeWriterOpt(outputStream, fileHandler.getProgramName(), comments);

    if (bootstrap) {
        codeWriterOpt->writeInit();

        if (verbose)
            cout << "bootstrap code written" << endl;
    }

    codeWriterOpt->writeGlobalFunctions();

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

        vmCmdVec.clear();
        vmArg2Vec.clear();

        Parser parser(inputFile);
        codeWriterOpt->setFileName(file);

        while (parser.advance()) {

            vmCmdVec.push_back(parser.commandOptType());

            if (parser.commandOptType() == VmCommandOptType::C_PUSH_CONSTANT)
                vmArg2Vec.push_back(parser.arg2());
            else
                vmArg2Vec.push_back(0);

        }

        parser.reset();
        newFileHasStarted = true;

        while (advanceParser(parser)) {

        // ??????????
        /*
        if (fileHandler.currentFile == "OS/Sys.vm") {

            bool loop = true;

            while (loop) {

                if (parser.commandType() == VmCommandType::C_CALL) {

                    string moduleName = FileHandler::getProgramName(parser.arg1());

                    if (calls.find(moduleName) == calls.end() && moduleName != "Main" && moduleName != "Sys") {
                        cout << "out: " << parser.arg1() << endl;
                        updateLookAhead(parser, 2, 1);
                        break;
                    }
                }

                updateLookAhead(parser, 1, 1);
                if (parser.commandType() == VmCommandType::C_RETURN)
                    loop = false;
            }

        }
        */

            if (parser.commandType() == VmCommandType::C_FUNCTION) {

                string arg1 = parser.arg1();
                string moduleName = arg1.substr(0, arg1.find(".")); // ???????????? FUNC

                if (funcCallMap.find(moduleName) != funcCallMap.end()) {

                    if (funcCallMap.at(moduleName).find(arg1) == funcCallMap.at(moduleName).end()) {

                        skippedFuncVec.push_back(arg1);

                        while (advanceParser(parser) && parser.commandType() != VmCommandType::C_RETURN);

                        continue;

                    }
                }
            }

            if (translateOptimisedSequence(parser, codeWriterOpt))
                continue;

            translateVmCommand(parser, codeWriterOpt);

        }

        fileCount++;

    }

    return fileCount;
}

void VmTranslator::printCalls()
{
    cout << "total entries in map: " << funcCallMap.size() << endl << endl;

    for (auto& it: funcCallMap) {

        cout << "module: " << it.first << endl;

        for (auto& setIt: it.second) {
            cout << "  + call " << setIt << endl;
        }

    }
}

bool VmTranslator::translateOptimisedSequence(Parser& parser, CodeWriterOpt* codeWriterOpt)
{
    // first look for push constant sequences

    if (vmCmdVec.size() >= 2) {

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_CONSTANT) {

            int constant = parser.arg2();
            int num = 1;

            while (vmCmdVec.at(num) == VmCommandOptType::C_PUSH_CONSTANT &&
                   vmArg2Vec.at(num) == constant)
                { num++; }

            if (num > 1) {

                codeWriterOpt->writePushConstantXTimes(constant, num);
                advanceParser(parser, num - 1);

                return true;

            }
        }
    }

    // second look for push constant + push local + add

    if (vmCmdVec.size() >= 3) {

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_CONSTANT &&
            vmCmdVec.at(1) == VmCommandOptType::C_PUSH_LOCAL &&
            vmCmdVec.at(2) == VmCommandOptType::C_ARITHMETIC_ADD)
            {
                int constant = parser.arg2();
                advanceParser(parser);
                int index = parser.arg2();
                codeWriterOpt->writeSeq3PushConstantPushLocalAdd(constant, index);
                advanceParser(parser);

                return true;
            }

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_LOCAL &&
            vmCmdVec.at(1) == VmCommandOptType::C_PUSH_CONSTANT &&
            vmCmdVec.at(2) == VmCommandOptType::C_ARITHMETIC_ADD)
            {
                int index = parser.arg2();
                advanceParser(parser);
                int constant = parser.arg2();

                codeWriterOpt->writeSeq3PushConstantPushLocalAdd(constant, index);
                advanceParser(parser);

                return true;
            }

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_LOCAL &&
            vmCmdVec.at(1) == VmCommandOptType::C_PUSH_STATIC &&
            vmCmdVec.at(2) == VmCommandOptType::C_ARITHMETIC_ADD)
            {
                int localIndex = parser.arg2();
                advanceParser(parser);
                int staticIndex = parser.arg2();
                codeWriterOpt->writeSeq3PushLocalPushStaticAdd(localIndex, staticIndex);
                advanceParser(parser);

                return true;
            }

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_LOCAL &&
            vmCmdVec.at(1) == VmCommandOptType::C_PUSH_LOCAL &&
            vmCmdVec.at(2) == VmCommandOptType::C_ARITHMETIC_ADD)
            {
                int local1Index = parser.arg2();
                advanceParser(parser);
                int local2Index = parser.arg2();
                codeWriterOpt->writeSeq3PushLocalPushLocalAdd(local1Index, local2Index);
                advanceParser(parser);

                return true;
            }

    }

    // else look for the following sequences...

    if (vmCmdVec.size() >= 2) {

        // starts with PUSH CONSTANT

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_CONSTANT &&
            vmCmdVec.at(1) == VmCommandOptType::C_ARITHMETIC_ADD)
            {
                codeWriterOpt->writeSeq2PushConstantAdd(parser.arg2());
                advanceParser(parser);

                return true;
            }

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_CONSTANT &&
            vmCmdVec.at(1) == VmCommandOptType::C_ARITHMETIC_SUB)
            {
                codeWriterOpt->writeSeq2PushConstantSub(parser.arg2());
                advanceParser(parser);

                return true;
            }

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_CONSTANT &&
            vmCmdVec.at(1) == VmCommandOptType::C_ARITHMETIC_NEG)
            {
                codeWriterOpt->writeSeq2PushConstantNeg(parser.arg2());
                advanceParser(parser);

                return true;
            }

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_CONSTANT &&
            (vmCmdVec.at(1) == VmCommandOptType::C_POP_ARGUMENT ||
             vmCmdVec.at(1) == VmCommandOptType::C_POP_LOCAL ||
             vmCmdVec.at(1) == VmCommandOptType::C_POP_THIS ||
             vmCmdVec.at(1) == VmCommandOptType::C_POP_THAT))
            {
                int constant = parser.arg2();
                advanceParser(parser);
                string segment = parser.arg1();
                int segIndex = parser.arg2();
                codeWriterOpt->writeSeq2PushConstantPopArgLocalThisThat(constant, segment, segIndex);

                return true;
            }

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_CONSTANT &&
            (vmCmdVec.at(1) == VmCommandOptType::C_POP_STATIC ||
             vmCmdVec.at(1) == VmCommandOptType::C_POP_TEMP))
            {
                int constant = parser.arg2();
                advanceParser(parser);
                string segment = parser.arg1();
                int segIndex = parser.arg2();
                codeWriterOpt->writeSeq2PushConstantPopStaticTemp(constant, segment, segIndex);

                return true;
            }

        // starts with PUSH ARGUMENT|LOCAL|THIS|THAT

        if ((vmCmdVec.at(0) == VmCommandOptType::C_PUSH_ARGUMENT ||
             vmCmdVec.at(0) == VmCommandOptType::C_PUSH_LOCAL ||
             vmCmdVec.at(0) == VmCommandOptType::C_PUSH_THIS ||
             vmCmdVec.at(0) == VmCommandOptType::C_PUSH_THAT) &&
             vmCmdVec.at(1) == VmCommandOptType::C_ARITHMETIC_ADD)
            {
                codeWriterOpt->writeSeq2PushArgLocalThisThatAdd(parser.arg1(), parser.arg2());
                advanceParser(parser);

                return true;
            }

        if ((vmCmdVec.at(0) == VmCommandOptType::C_PUSH_ARGUMENT ||
             vmCmdVec.at(0) == VmCommandOptType::C_PUSH_LOCAL ||
             vmCmdVec.at(0) == VmCommandOptType::C_PUSH_THIS ||
             vmCmdVec.at(0) == VmCommandOptType::C_PUSH_THAT) &&
             vmCmdVec.at(1) == VmCommandOptType::C_ARITHMETIC_SUB)
            {
                codeWriterOpt->writeSeq2PushArgLocalThisThatSub(parser.arg1(), parser.arg2());
                advanceParser(parser);

                return true;
            }

        // starts with PUSH TEMP

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_TEMP &&
            vmCmdVec.at(1) == VmCommandOptType::C_POP_THAT)
            {
                int tempIndex = parser.arg2();
                advanceParser(parser);
                int thatIndex = parser.arg2();
                codeWriterOpt->writeSeq2PushTempPopThat(tempIndex, thatIndex);

                return true;
            }

        // starts with PUSH ARGUMENT

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_ARGUMENT &&
            vmCmdVec.at(1) == VmCommandOptType::C_POP_POINTER)
            {
                int argIndex = parser.arg2();
                advanceParser(parser);
                int pointerIndex = parser.arg2();
                codeWriterOpt->writeSeq2PushArgumentPopPointer(argIndex, pointerIndex);

                return true;
            }

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_ARGUMENT &&
            vmCmdVec.at(1) == VmCommandOptType::C_POP_TEMP)
            {
                int argIndex = parser.arg2();
                advanceParser(parser);
                int tempIndex = parser.arg2();
                codeWriterOpt->writeSeq2PushArgumentPopTemp(argIndex, tempIndex);

                return true;
            }

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_ARGUMENT &&
            (vmCmdVec.at(1) == VmCommandOptType::C_POP_ARGUMENT ||
             vmCmdVec.at(1) == VmCommandOptType::C_POP_LOCAL))
            {
                int argIndex = parser.arg2();
                advanceParser(parser);
                string segment = parser.arg1();
                int segIndex = parser.arg2();
                codeWriterOpt->writeSeq2PushArgumentPopArgLocal(argIndex, segment, segIndex);

                return true;
            }

        // starts with PUSH STATIC

        if (vmCmdVec.at(0) == VmCommandOptType::C_PUSH_STATIC &&
            vmCmdVec.at(1) == VmCommandOptType::C_ARITHMETIC_ADD)
            {
                codeWriterOpt->writeSeq2PushStaticAdd(parser.arg2());
                advanceParser(parser);

                return true;
            }
    }

    return false;
}

void VmTranslator::updateLookAhead(Parser& parser, int skipVectores, int skipParser)
{
    vmCmdVec.erase(vmCmdVec.begin(), vmCmdVec.begin() + skipVectores);
    vmArg2Vec.erase(vmArg2Vec.begin(), vmArg2Vec.begin() + skipVectores);

    for (int i = 0; i < skipParser; i++)
        parser.advance();
}

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

void VmTranslator::scanFunctionCalls(string module, string function)
{
    string path = fileHandler.getPath();
    ifstream inStream(path + module + ".vm");

    if (!inStream.good())
        return;

    Parser parser(inStream);

    while (parser.advance()) {

        if (parser.commandType() == VmCommandType::C_FUNCTION &&
            parser.arg1() == function)
            {
                while (parser.advance() && parser.commandType() != VmCommandType::C_RETURN) {

                    if (parser.commandType() == VmCommandType::C_CALL) {
                        string arg1 = parser.arg1();
                        string mod = arg1.substr(0, arg1.find("."));

                        if (funcCallMap.find(mod) == funcCallMap.end()) { // module first entry
                            set<string> funcSet;
                            funcCallMap.insert(pair<string, set<string> >(mod, funcSet));
                            funcCallMap.at(mod).insert(arg1);
                            scanFunctionCalls(mod, arg1);
                            scanFunctionCalls(mod, mod + ".init"); // looks for a .init function
                            continue;
                        }

                        if (funcCallMap.at(mod).find(arg1) != funcCallMap.at(mod).end()) // already has mod and func
                            continue;

                        funcCallMap.at(mod).insert(arg1);
                        scanFunctionCalls(mod, arg1);
                    }
                }
            }
    }

    if (inStream.is_open())
        inStream.close();
}
