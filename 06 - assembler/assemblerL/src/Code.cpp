#include "Code.h"
#include <algorithm>

unsigned int Code::dest(string mnemonic)
{
	// format 0000000000ddd000

	unsigned int dest = 0;

	if (mnemonic.find('M') != string::npos) dest |= 0b001;
	if (mnemonic.find('D') != string::npos) dest |= 0b010;
	if (mnemonic.find('A') != string::npos) dest |= 0b100;

	return dest << 3;
}

unsigned int Code::comp(string mnemonic)
{
	// format 000ccccccc000000

	unsigned int mask = 0;
	unsigned int comp;

	if (mnemonic.find('M') != string::npos) {
		mask = 64; // 0b1000000
		replace(mnemonic.begin(), mnemonic.end(), 'M', 'A');
	}

	if (mnemonic == "0") 		comp = 0b101010;
	else if (mnemonic == "1")  	comp = 0b111111;
	else if (mnemonic == "-1")  comp = 0b111010;
	else if (mnemonic == "D")   comp = 0b001100;
	else if (mnemonic == "A")   comp = 0b110000;
	else if (mnemonic == "!D")  comp = 0b001101;
	else if (mnemonic == "!A")  comp = 0b110001;
	else if (mnemonic == "-D")  comp = 0b001111;
	else if (mnemonic == "-A")  comp = 0b110011;
	else if (mnemonic == "D+1") comp = 0b011111;
	else if (mnemonic == "A+1") comp = 0b110111;
	else if (mnemonic == "D-1") comp = 0b001110;
	else if (mnemonic == "A-1") comp = 0b110010;
	else if (mnemonic == "D+A") comp = 0b000010;
	else if (mnemonic == "D-A") comp = 0b010011;
	else if (mnemonic == "A-D") comp = 0b000111;
	else if (mnemonic == "D&A") comp = 0b000000;
	else 						comp = 0b010101;

	return (comp |= mask) << 6;
}

unsigned int Code::jump(string mnemonic)
{
	// format 0000000000000jjj

	if (mnemonic == "JGT") return 0b001;
	if (mnemonic == "JEQ") return 0b010;
	if (mnemonic == "JGE") return 0b011;
	if (mnemonic == "JLT") return 0b100;
	if (mnemonic == "JNE") return 0b101;
	if (mnemonic == "JLE") return 0b110;
	if (mnemonic == "JMP") return 0b111;

	return 0; // null case
}
