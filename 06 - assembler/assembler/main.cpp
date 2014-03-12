#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include "Assembler.h"
#include "FileHandler.h"

using namespace std;

/**
	Gets the verbose and 'very verbose' flags from the command line arguments.

	@param argc Number of command line arguments.

	@param argv Array of arguments.

	@param verbose Verbose flag. Its value will be affected by the arguments.

	@param veryVerbose 'Very verbose' flag. Its value will be affected by the arguments.

	@param symTable Output symbol table flag. Its value will be affected by the arguments.

	@return Returns false if an unidentified flag is present in the arguments.
	True otherwise.
*/
bool getFlags(int argc, char** argv, bool& verbose, bool& veryVerbose, bool& symTable)
{
	char c;
	verbose = veryVerbose = symTable = false;

	while ((c = getopt(argc, argv, "vVt")) != -1) {

        switch (c) {

            case 'V':
                veryVerbose = true;
            case 'v':
                verbose = true;
                break;

            case 't':
                symTable = true;
                break;

            case '?':
                return 1;

        }
    }

	return true;
}

/**
	Checks if the input string is the name of a regular file with the extension ".asm".

	@return True if the input string is the name of .asm file. False otherwise.
*/
bool isAsmFile(string input)
{
	if (!FileHandler::isFile(input)) {
		cerr << "error: input \"" << input << "\"is not a file" << endl;
		return false;
	}

	if (!FileHandler::hasExtension(input, ".asm")) {
		cerr << "error: input file must have .asm extension" << endl;
		return false;
	}

	return true;
}

/**

*/
int main(int argc, char** argv)
{
	if (argc == 1) {
		cerr << "usage: " << "hass" << " [-v|-V|-t] input-filename.asm $test$" << endl;
		cerr << "       -v verbose" << endl;
		cerr << "       -V very verbose" << endl;
		cerr << "       -t output symbol table" << endl;
		return 1;
	}

	bool verbose, veryVerbose, symTable;

	if (!getFlags(argc, argv, verbose, veryVerbose, symTable))
		return 1;

	string inputName(argv[argc - 1]);

	if (!isAsmFile(inputName))
		return 1;

	ifstream inputFile(inputName);

	if (!inputFile.good()) {
		cerr << "error: unable to open input stream" << endl;
		return 1;
	}

	string outputName(FileHandler::changeExtension(inputName, ".hack"));
	ofstream outputFile(outputName);

	if (!outputFile.good()) {
		cerr << "error: unable to open output stream" << endl;
		return 1;
	}

	Assembler hass(inputFile, inputName, outputFile, outputName, verbose, veryVerbose);
	hass.assemble();

	if (symTable) {
		string symOutputName(FileHandler::changeExtension(inputName, "-symbols"));
		ofstream symOutputFile(symOutputName);

		if (!symOutputFile.good()) {
			cerr << "error: unable to open output stream" << endl;
			return 1;
		}

		hass.outputSymbolTable(symOutputFile);
		symOutputFile.close();

		if (verbose)
			cout << "symbol table output: " << symOutputName << endl;
	}

	if (inputFile.is_open())
		inputFile.close();

	if (outputFile.is_open())
		outputFile.close();

	return 0;
}
