#ifndef ASSEMBLER_INCLUDED_H
#define ASSEMBLER_INCLUDED_H

#include "Parser.h"
#include "Code.h"
#include "SymbolTable.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;

class Assembler
{

	public:

     	/**
			Constructs an Assembler object and binds it to the input and output stream.

            @param inputStream Input stream with the Hack assembly program.

			@param inputName Name of the input stream.

            @param outputStream Output stream in which the assembler will write Hack machine code.

            @param outputName Name of the output stream.

            @param verbose Flags the assembler to switch verbose mode on or off.

            @param veryVerbose Flags the assembler to switch 'very verbose' mode on or off.
		*/
        Assembler(istream& inputStream, string inputName, ostream& outputStream, string outputName,
        	      bool verbose = false, bool veryVerbose = false);

		/**
			Assembles the input stream into Hack machine code and writes it to the output stream.
			If the assembler is in verbose or 'very verbose' mode, a call to assemble() will print
			details of the assembling process to the standard output.
		*/
		void assemble();

		/**
			First pass of the assembling process. Populates the symbol table with the program's labels only.
			Does not generate code.

			If the assembler is in verbose mode, a call to assembleFirstPass() will print details of the
			assembling process to the standard output.
		*/
		void assembleFirstPass();

		/**
			Second pass of the assembling process. Writes Hack machine code to the output stream and
			populates the symbol table with the program's variables.

			If the assembler is in verbose mode, a call to assembleFirstPass() will print details of the
			assembling process to the standard output.
		*/
		void assembleSecondPass();

		/**
			Outputs to a file the symbol table of the program being assembled. The output file will be named
			"prog-symbols", being "prog" the name of the output stream.

			@return True if the symbol table was succesfully outputed to a file. False if an error occurred
			while opening the output file.
		*/
		void outputSymbolTable(ostream& symOutputStream);

		/**
			Sets the verbose mode on or off.

			@param verbose True for verbose mode on, False for verbose mode off.
		*/
		void setVerbose(bool verbose);

		/**
			Is verbose mode turned on or off?

			@return True if verbose mode is on. False otherwise.
		*/
		bool isVerbose();

		/**
			Sets the 'very verbose' mode on or off. This mode will print out details related to C-instructions.

			@note Setting 'very verbose' will automatically set verbose mode on.

			@param veryVerbose True for verbose mode on, False for verbose mode off.
		*/
		void setVeryVerbose(bool veryVerbose);

		/**
			Is 'very verbose' mode turned on or off?

			@return True if 'very verbose' mode is on. False otherwise.
		*/
		bool isVeryVerbose();

	private:

		/**
			Populates the @ref symbolTable without generating code. The first pass will only compute the
			program's labels. The program's variable are handled in the second pass.
		*/
		void firstPass();

		/**
			Generates the Hack machine code to the @ref outputStream using the symbol table to map labels
			to ROM addresses. Variables are added to the @ref symbolTable during the second pass, mapping
			variables to RAM addresses.
		*/
		void secondPass();

		/**
			Assembles an A-instruction (A_COMMAND), writing to the output stream its machine code.
		*/
		void assembleACommand();

		/**
			Assembles a C-instruction (C_COMMAND), writing to the output stream its machine code.
		*/
		void assembleCCommand();

		/**
			Prints a header (with command details) and assembling information to the standard output.
		*/
		void printHeader();

		/**
			Prints the command header.
		*/
		void printCmdHeader();

		/**
			Prints details of the current command being assembled. If 'very verbose'
			mode is on, this method will print extra details related to C-instructions.
		*/
		void printCmdDetails();

		/**
			Populates @ref predefinedMap with Hack predefined symbols.
		*/
		void mapPredefinedSymbols();

		istream& inputStream;			/**< Input stream with the Hack assembly program. */

		ostream& outputStream;			/**< Output stream which the assembler writes the Hack machine code. */

		string inputName;				/**< Name of the input stream. */

		string outputName; 				/**< Name of the output stream. */

		string bin;						/**< Binary string of the last assembled instruction. */

		Parser parser; 					/**< To read/parse the input file. */

		Code code; 						/**< To translate the input from the parser. */

		SymbolTable symbolTable; 		/**< To handle labels and variables. */

		bool verbose; 					/**< Flag for the verbose mode. */

		bool veryVerbose; 				/**< Flag for the 'very verbose' mode. */

		map<string, int> predefinedMap; /**< Set with the predefined symbols of the Hack language. */

		const string assemblerTitle; 	/**< Assembler title. */

		const string assemblerSubtitle; /**< Assembler subtitle. */

		const string assemblerVersion;	/**< Assembler version. */

};

#endif // ASSEMBLER_INCLUDED_H
