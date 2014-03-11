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
	  assemblerSubtitle("hack assembler (nand2tetris, chap. 6)"),
	  assemblerVersion("0.3")
{
	mapPredefinedSymbols();
}

void Assembler::assemble()
{
	assembleFirstPass();
	assembleSecondPass();
	if (verbose) cout << "done" << endl << endl;
}

void Assembler::assembleFirstPass()
{
	if (verbose) printHeader();
	firstPass();
}

void Assembler::assembleSecondPass()
{
	if (verbose) printCmdHeader();
	parser.reset();
	secondPass();
}

void Assembler::outputSymbolTable(ostream& symOutputStream)
{
	if (!predefinedMap.empty()) { // if there are predefined symbols, print them
		symOutputStream << "**** predefined symbols:" << endl << endl;

		for (map<string,int>::iterator it = predefinedMap.begin(); it != predefinedMap.end(); it++)
			symOutputStream << "0x" << setfill('0') << setw(4) << setbase(16) << it->second << " " << it->first << endl;

		symOutputStream << endl;
	}

	map<string, int> table = symbolTable.getTable();
	symOutputStream << "**** " << outputName << " symbols:" << endl << endl;

	for (map<string,int>::iterator it = table.begin(); it != table.end(); it++) {
		if (predefinedMap.find(it->first) != predefinedMap.end())
			continue;

		symOutputStream << "0x" << setfill('0') << setw(4) << setbase(16) << it->second << " " << it->first << endl;
	}
}

void Assembler::setVerbose(bool verbose)
{
	this->verbose = verbose;
}

bool Assembler::isVerbose()
{
	return verbose;
}

void Assembler::setVeryVerbose(bool veryVerbose)
{
	this->veryVerbose = veryVerbose;
	if (veryVerbose)
		verbose = true;
}

bool Assembler::isVeryVerbose()
{
	return veryVerbose;
}

void Assembler::firstPass()
{
	int num = 0;
	bool symAdded = false;
	string symbol;

	while (parser.advance()) {

		switch (parser.commandType()) {

			case HasmCommandType::L_COMMAND:
				symbol = parser.symbol();
				symbolTable.addEntry(symbol, num);

				if (verbose) {
					if (!symAdded) {
						cout << "symbols from first pass:" << endl;
						symAdded = true;
					}

					cout << setfill('0');
					cout << symbol << " mapped to address 0x" << setw(4) << setbase(16) << num << endl;
					cout << setfill(' ');
				}

				break;

			default:
				num++;

		}
	}

	if (verbose && symAdded) cout << endl;
}

void Assembler::secondPass()
{
	while (parser.advance()) {

		switch (parser.commandType()) {

			case HasmCommandType::A_COMMAND:
				assembleACommand();
				break;

			case HasmCommandType::C_COMMAND:
				assembleCCommand();
				break;

			case HasmCommandType::L_COMMAND:
				break;

		}

		if (verbose) printCmdDetails();

	}
}

void Assembler::assembleACommand()
{
	static int RAMadr = 16;
	string symbol = parser.symbol();
	int val;

	if (isdigit(symbol.front())) {

		val = stoi(parser.symbol());

	} else {

		if (symbolTable.contains(symbol)) {
			val = symbolTable.getAddress(symbol);
		} else {
			symbolTable.addEntry(symbol, RAMadr);
			val = RAMadr++;
		}

	}

	bin = bitset< 16 >(val).to_string();
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

void Assembler::mapPredefinedSymbols()
{
	predefinedMap.insert(pair<string, int>("SP",   0x0000));
	predefinedMap.insert(pair<string, int>("LCL",  0x0001));
	predefinedMap.insert(pair<string, int>("ARG",  0x0002));
	predefinedMap.insert(pair<string, int>("THIS", 0x0003));
	predefinedMap.insert(pair<string, int>("THAT", 0x0004));

	for (int i = 0; i < 16; i++) {
		string reg("R");
		reg.append(to_string(i));
		predefinedMap.insert(pair<string, int>(reg, i));
	}

	predefinedMap.insert(pair<string, int>("SCREEN", 0x4000));
	predefinedMap.insert(pair<string, int>("KBD",    0x6000));

	symbolTable.loadMap(predefinedMap);
}
