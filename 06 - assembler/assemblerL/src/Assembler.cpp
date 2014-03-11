#include "Assembler.h"
#include <iomanip>
#include <bitset>

Assembler::Assembler(istream& inputStream, string inputName, ostream& outputStream, string outputName,
	                 bool verbose, bool veryVerbose)

	: inputStream(inputStream),
	  outputStream(outputStream),
	  inputName(inputName),
	  outputName(outputName),
	  parser(inputStream),
	  verbose(verbose),
	  veryVerbose(veryVerbose),
	  assemblerTitle("hass"),
	  assemblerSubtitle("hack assembler symbol-less version (nand2tetris, chap. 6)"),
	  assemblerVersion("0.3")
{
}

void Assembler::assemble()
{
	if (verbose) {
		printHeader();
		printCmdHeader();
	}

	while (parser.advance()) {

		switch (parser.commandType()) {

			case HasmCommandType::A_COMMAND:
				assembleACommand();
				break;

			case HasmCommandType::C_COMMAND:
				assembleCCommand();

			case HasmCommandType::L_COMMAND:
				break;

		}

		if (verbose) printCmdDetails();

	}

	if (verbose) cout << "done" << endl << endl;
}

void Assembler::assembleACommand()
{
	bin = bitset< 16 >(stoi(parser.symbol())).to_string();
	outputStream << bin << endl;
}

void Assembler::assembleCCommand()
{
	string dest = parser.dest();
	string comp = parser.comp();
	string jump = parser.jump();
	unsigned int cc = 0; // c_command

	cc = code.dest(dest);
	cc |= code.comp(comp);
	cc |= code.jump(jump);
	cc |= 0b1110000000000000;

	bin = bitset< 16 >(cc).to_string();
	outputStream << bin << endl;
}

void Assembler::printHeader()
{
	cout << assemblerTitle << " - " << assemblerSubtitle << " v" << assemblerVersion << endl;
	cout << "input: " << inputName << endl;
	cout << "output: " << outputName << endl << endl;
}

void Assembler::printCmdHeader()
{
	cout << "#num  #pos  cmd" << setw(29) << "type       bin" << endl;
}

void Assembler::printCmdDetails()
{
	static int count = 1;
	cout << setfill('0');
	cout << setw(4) << right << setbase(16) << count++ << " ";
	cout << "[" << setw(4) << setbase(16) << parser.getLinePos() << "] ";
	cout << setfill(' ');
	cout << setw(18) << left << parser.getCommand();

	switch (parser.commandType()) {

        case HasmCommandType::A_COMMAND:
            cout << "A_COMMAND  ";
            cout << bin << endl;
            break;

        case HasmCommandType::L_COMMAND:
            cout << "L_COMMAND  " << endl;
            break;

        case HasmCommandType::C_COMMAND:
            cout << "C_COMMAND  " << bin;

            if (veryVerbose) {
                string dest = parser.dest();
                string comp = parser.comp();
                string jump = parser.jump();
                cout << " ";
                if (!dest.empty()) cout << "dest: " << dest << " ";
                if (!comp.empty()) cout << "comp: " << comp << " ";
                if (!jump.empty()) cout << "jump: " << jump;
            }

            cout << endl;

    }
}
