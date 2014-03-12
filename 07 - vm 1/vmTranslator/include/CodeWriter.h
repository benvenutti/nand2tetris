#ifndef CODE_WRITER_INCLUDED_H
#define CODE_WRITER_INCLUDED_H

#include "VmCommandType.h"
#include <iostream>
#include <string>

using namespace std;

class CodeWriter
{

	public:

		/**
			Constructs a CodeWriter object and binds it to the output stream.

			@param outputStream Output stream where Hack assembly code will be written.

			@param programName Name of the program.
		*/
		CodeWriter(ostream& outputStream, string programName);

		/**
			Informs the code writer that the translation of a new VM file has started.

			@param fileName String with the name of the VM file.
		*/
		void setFileName(string fileName);

		/**
			Writes the assembly code that is the translation of the given arithmetic command.

			@param command String with an arithmetic VM command.
		*/
		void writeArithmetic(string command);

		/**
			Writes the assembly code that is the translation of the given command, where command is
			either C_PUSH or C_POP.

			@param VmCommandType VmCommandType (C_PUSH or C_POP).

			@param segment String with the name of the segment being pushed or popped.

			@param index Index of the segment.
		*/
		void writePushPop(VmCommandType cmd, string segment, int index);

	private:

		/**
			Writes Hack assembly code for the 'add' arithmetic VM command.
		*/
		void writeArithmeticAdd();

		/**
			Writes Hack assembly code for the 'sub' arithmetic VM command.
		*/
		void writeArithmeticSub();

		/**
			Writes Hack assembly code for the 'neg' arithmetic VM command.
		*/
		void writeArithmeticNeg();

		/**
			Writes Hack assembly code for the 'eq' arithmetic VM command.
		*/
		void writeArithmeticEq();

		/**
			Writes Hack assembly code for the 'gt' arithmetic VM command.
		*/
		void writeArithmeticGt();

		/**
			Writes Hack assembly code for the 'lt' arithmetic VM command.
		*/
		void writeArithmeticLt();

		/**
			Writes Hack assembly code for the 'and' arithmetic VM command.
		*/
		void writeArithmeticAnd();

		/**
			Writes Hack assembly code for the 'or' arithmetic VM command.
		*/
		void writeArithmeticOr();

		/**
			Writes Hack assembly code for the 'not' arithmetic VM command.
		*/
		void writeArithmeticNot();

		/**
			Writes Hack assembly code for the push VM command.

			@param segment String with the name of the segment which data will be pushed
			to the stack.

			@param index Index of the data being pushed.
		*/
		void writePush(string segment, int index);

		/**
			Writes Hack assembly code for the pop VM command.

			@param segment String with the name of the segment which will receive data
			from the stack.

			@param index Index of the data receiving data from the stack.
		*/
		void writePop(string segment, int index);

		/**
			Returns a string with the Hack register symbol related to a given segment.

			@param segment String with the name of the segment.

			@param index Index of the segment.

			@return String with the Hack (pre defined) symbol for the given segment.
		*/
		string registerName(string segment,int index);

		/**
			Writes a program comment header to the output stream. Called by the constructor.
		*/
		void writeProgramHeader();

		/**
			Writes a module comment header to the ouput stream. Called by setFileName(string).
		*/
		void writeModuleHeader();

		/**
			Writes a given string to the output stream. If the ident parameter @param ident is
			true, the string will be idented by a tab.

			@param code String to be written to the output stream.

			@param ident If true, the written code will be idented (tabbed).
		*/
		void write(string code, bool ident = true);

        ostream& outputStream; 			/**< The output stream where Hack assembly code will be written. */

		string programName;				/**< Name of the program. */

		string fileName; 				/**< Name of the VM being translated. */

		string moduleName; 				/**< Name of of the current module (the @ref fileName without
										its extension). */

		int symbolCounter; 				/**< Counter of symbols in the program. */

};

#endif // CODE_WRITER_INCLUDED_H
