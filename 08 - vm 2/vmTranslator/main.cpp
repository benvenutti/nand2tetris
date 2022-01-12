#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include "VmTranslator.h"
#include "FileHandler.h"

using namespace std;
/*
int main(int argc, char** argv)
{
	bool verbose = false;
	bool bootstrap = true;
	bool comments = true;
	bool optimise = true;

	string input("OS_0/");

	if (!FileHandler::isFile(input) && !FileHandler::isDir(input)) {
		cerr << "error: invalid input \"" << input << "\"" << endl;
		return 1;
	}

	string outputFileName(FileHandler::getOutputFile(input));
	ofstream outputFile(outputFileName);

	if (!outputFile.good()) {
		cerr << "error: unable to open file \"" << outputFileName << "\"" << endl;
		return 1;
	}

	VmTranslator vmTranslator(input, outputFile, verbose, bootstrap, comments, optimise);
	vmTranslator.translate();
	outputFile.close();

	return 0;
}
*/

int main(int argc, char** argv)
{
	if (argc == 1) {
		cerr << "usage: vm2hack [-v|-b|-c|-o] fileName.vm|directoryName" << endl;
		cerr << "       -v verbose" << endl;
		cerr << "       -b bootstrap" << endl;
		cerr << "       -c comment .asm file" << endl;
		cerr << "       -o optimisation" << endl;
		return 1;
	}

	char c;
	bool verbose = false;
	bool bootstrap = false;
	bool comments = false;
	bool optimise = false;

	while ((c = getopt(argc, argv, "vbco")) != -1) {

        switch (c) {

            case 'v':
                verbose = true;
                break;

            case 'b':
            	bootstrap = true;
            	break;

            case 'c':
            	comments = true;
            	break;

            case 'o':
            	optimise = true;
            	break;

            case '?':
                return 1;

        }
    }

	string input(argv[argc - 1]);

	if (!FileHandler::isFile(input) && !FileHandler::isDir(input)) {
		cerr << "error: invalid input \"" << input << "\"" << endl;
		return 1;
	}

	string outputFileName(FileHandler::getOutputFile(input));
	ofstream outputFile(outputFileName);

	if (!outputFile.good()) {
		cerr << "error: unable to open file \"" << outputFileName << "\"" << endl;
		return 1;
	}

	VmTranslator vmTranslator(input, outputFile, verbose, bootstrap, comments, optimise);
	vmTranslator.translate();
	outputFile.close();

	return 0;
}

