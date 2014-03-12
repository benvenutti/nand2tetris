#include "VmTranslator.h"
#include "VmCommandType.h"
#include "Parser.h"
#include "CodeWriter.h"
#include <fstream>

VmTranslator::VmTranslator(string input, ostream& outputStream, bool verbose)
	: input(input), outputStream(outputStream), fileHandler(input),
	  verbose(verbose),
	  vmTransTitle("vm2hack"),
	  vmTransSubtitle("partial vm translator (nand2tetris, chap. 7)"),
	  vmTransVersion("0.2")
{
}

void VmTranslator::translate()
{
	if (verbose) printHeader();

	CodeWriter codeWriter(outputStream, fileHandler.getProgramName());
	string file;

	int count = 0;

	while (fileHandler.nextVmFile(file)) {

		if (verbose) {
			cout << "#" << ++count << " " << file << " -> module: ";
			cout << FileHandler::getProgramName(file) << endl;
		}

		ifstream inputFile(file);
		if (!inputFile.good()) {
			cerr << "error: unable to open file \"" << file << "\"" << endl;
			continue;
		}

		Parser parser(inputFile);
		codeWriter.setFileName(file);

		while (parser.advance()) {

			switch (parser.commandType()) {

				case VmCommandType::C_ARITHMETIC:
					codeWriter.writeArithmetic(parser.getCommand());
					break;

				case VmCommandType::C_PUSH:
				case VmCommandType::C_POP:
					codeWriter.writePushPop(parser.commandType(), parser.arg1(), parser.arg2());
					break;

				default:
					break;
			}

		}

	}

	if (verbose) {
		cout << endl;
		cout << "Number of files translated: " << count << endl;
		cout << "done" << endl << endl;
	}
}

void VmTranslator::printHeader()
{
	cout << vmTransTitle << " - " << vmTransSubtitle << " v" << vmTransVersion << endl;
	cout << "input: " << input;

	if (fileHandler.isDir()) {
		cout << " (directory with " << fileHandler.getNumVmFiles();
		fileHandler.getNumVmFiles() == 1 ? cout << " vm file)" : cout << " vm files)";
	}

	cout << endl;
	cout << "output: " << fileHandler.getOutputFile() << endl << endl;
}
