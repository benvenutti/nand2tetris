#ifndef ASSEMBLER_INCLUDED_H
#define ASSEMBLER_INCLUDED_H

#include <iostream>
#include <string>
#include "Parser.h"
#include "Code.h"

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

	private:

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

		istream& inputStream;			/**< Input stream with the Hack assembly program. */

		ostream& outputStream;			/**< Output stream which the assembler writes the Hack machine code. */

		string inputName;				/**< Name of the input stream. */

		string outputName; 				/**< Name of the output stream. */

		string bin;						/**< Binary string of the last assembled instruction. */

		Parser parser; 					/**< To read/parse the input file. */

		Code code; 						/**< To translate the input from the parser. */

		bool verbose; 					/**< Flag for the verbose mode. */

		bool veryVerbose; 				/**< Flag for the 'very verbose' mode. */

		const string assemblerTitle; 	/**< Assembler title. */

		const string assemblerSubtitle; /**< Assembler subtitle. */

		const string assemblerVersion;	/**< Assembler version. */

};

#endif // ASSEMBLER_INCLUDED_H
