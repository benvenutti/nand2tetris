#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include "VmTranslator.h"
#include "FileHandler.h"

using namespace std;

int main(int argc, char** argv)
{
	if (argc == 1) {
		cerr << "usage: vm2hack [-v] fileName.vm|directoryName" << endl;
		cerr << "       -v verbose" << endl;
		return 1;
	}

	char c;
	bool verbose = false;
	int inputArg = 1;

	while ((c = getopt(argc, argv, "v:")) != -1) {

        switch (c) {

            case 'v':
                verbose = true;
                inputArg++;
                break;

            case '?':
                return 1;

        }
    }

	string input(argv[inputArg]);

	if (!FileHandler::isFile(input) && !FileHandler::isDir(input)) {
		cerr << "error: invalid input" << endl;
		return 1;
	}

	string outputFileName(FileHandler::getOutputFile(input));
	ofstream outputFile(outputFileName);

	if (!outputFile.good()) {
		cerr << "error: unable to open file" << endl;
		return 1;
	}

	VmTranslator vmTranslator(input, outputFile, verbose);
	vmTranslator.translate();
	outputFile.close();

	return 0;
}
