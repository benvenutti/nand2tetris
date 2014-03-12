#include <string>
#include <iostream>
#include "FileHandler.h"
#include "CompilationEngine.h"

using namespace std;

const string title = "jparser";
const string version = "0.2a";

int main(int argc, char** argv)
{
	if (argc == 1 || argc > 3) {
		cerr << "usage: " << title << " <filename of directory>" << endl;
		return 1;
	}

	string input(argv[1]);
	cout << title << " v" << version << endl;
	cout << "input: " << input;

	FileHandler fh(input);

	if (fh.isFile())
		cout << " (file)" << endl << endl;
	else if (fh.isDir())
		cout << " (directory with " << fh.getNumJackFiles() << " jack files)" << endl << endl;
	else {
		cerr << " (ERROR: not a file nor a directory)" << endl;
		cerr << "exiting..." << endl;
		return 1;
	}

	string file;
	int cont = 1;
	int error = 0;

	while (fh.nextJackFile(file)) {
		CompilationEngine compEng(file, fh.getOutputFile(file));

	    cout << "#" << cont++ << " parsing " << file << endl;

	    if (compEng.compile()) {
	        cout << "output: " << fh.getOutputFile(file) << endl << endl;
	    } else {
	    	error++;
	        cerr << "error while parsing class" << endl;
	        cerr << "partial output: " << fh.getOutputFile(file) << endl << endl;
	    }
	}

	if (error == 0)
		cout << fh.getNumJackFiles() << " jack file(s) successfully parsed" << endl;
	else
		cerr << "problems while parsing " << error << " jack file(s)" << endl;

	cout << "done" << endl;

    return 0;
}
