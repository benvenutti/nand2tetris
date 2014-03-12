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

			@param writeComments Flags the CodeWriter to output comments (original VM commands plus
			program and module headers and other comments) on the .asm file.
		*/
		CodeWriter(ostream& outputStream, string programName, bool writeComments);

		/**
			Informs the code writer that the translation of a new VM file has started.

			@param fileName String with the name of the VM file.
		*/
		virtual void setFileName(string fileName);

		/**
			Writes the assembly code that is the translation of the given arithmetic command.

			@param command String with an arithmetic VM command.
		*/
		virtual void writeArithmetic(string command);

		/**
			Writes the assembly code that is the translation of the given command, where command is
			either C_PUSH or C_POP.

			@param VmCommandType VmCommandType (C_PUSH or C_POP).

			@param segment String with the name of the segment being pushed or popped.

			@param index Index of the segment.
		*/
		virtual void writePushPop(VmCommandType cmd, string segment, int index);

		/**
			Writes assembly code that effects the VM initialization, also called <I>bootstrap
			code</I>. This code must be placed at the beginning of the output file.
		*/
		virtual void writeInit();

		/**
			Writes assembly code that effects the label command.

			@param label String with label.
		*/
		virtual void writeLabel(string label);

		/**
			Writes assembly code that effects the goto command.

			@param label String with label to where the command will jump to.
		*/
		virtual void writeGoto(string label);

		/**
			Writes assembly code that effects the if-goto command.

			@param label String with label to where the command may jump to.
		*/
		virtual void writeIf(string label);

		/**
			Writes assembly code that effects the call command.

			@param functionName String with the name of the function to call.

			@param numArgs Number of arguments required by the function.
		*/
		virtual void writeCall(string functionName, int numArgs);

		/**
			Writes assembly code that effects the return command.
		*/
		virtual void writeReturn();

		/**
			Writes assembly code that effects the function command.

			@param functionName String with the name of the function.

			@param numLocals Number of local variables required by the function.
		*/
		virtual void writeFunction(string functionName, int numLocals);

	protected:

		/**
			Writes Hack assembly code for the 'add' arithmetic VM command.
		*/
		virtual void writeArithmeticAdd();

		/**
			Writes Hack assembly code for the 'sub' arithmetic VM command.
		*/
		virtual void writeArithmeticSub();

		/**
			Writes Hack assembly code for the 'neg' arithmetic VM command.
		*/
		virtual void writeArithmeticNeg();

		/**
			Writes Hack assembly code for the 'eq' arithmetic VM command.
		*/
		virtual void writeArithmeticEq(); // virt

		/**
			Writes Hack assembly code for the 'gt' arithmetic VM command.
		*/
		virtual void writeArithmeticGt(); // virt

		/**
			Writes Hack assembly code for the 'lt' arithmetic VM command.
		*/
		virtual void writeArithmeticLt(); // virt

		/**
			Writes Hack assembly code for the 'and' arithmetic VM command.
		*/
		virtual void writeArithmeticAnd();

		/**
			Writes Hack assembly code for the 'or' arithmetic VM command.
		*/
		virtual void writeArithmeticOr();

		/**
			Writes Hack assembly code for the 'not' arithmetic VM command.
		*/
		virtual void writeArithmeticNot();

		/**
			Writes Hack assembly code for the push VM command.

			@param segment String with the name of the segment which data will be pushed
			to the stack.

			@param index Index of the data being pushed.
		*/
		virtual void writePush(string segment, int index); // virt

		/**
			Writes Hack assembly code for the pop VM command.

			@param segment String with the name of the segment which will receive data
			from the stack.

			@param index Index of the data receiving data from the stack.
		*/
		virtual void writePop(string segment, int index);

		/**
			Writes a program comment header to the output stream. Called by the constructor.
		*/
		virtual void writeProgramHeader();

		/**
			Writes a module comment header to the ouput stream. Called by setFileName(string).
		*/
		virtual void writeModuleHeader();

		/**
			Writes a given string with code to the output stream. If the ident parameter @param ident
			is true, the string will be idented by a tab.

			@param code String to be written to the output stream.

			@param ident If true, the written code will be idented (tabbed).
		*/
		virtual void write(string code, bool ident = true);

		/**
			Writes a given string with code to the output stream. If private member @ref writeComments
			is true, the supplied string @param comment will also be written to the output stream. Also,
			if the ident parameter @param ident is true, the string will be idented by a tab.

			@param code String to be written to the output stream.

			@param comment String with comment that may be written to the output stream.

			@param ident If true, the written code will be idented (tabbed).
		*/
		virtual void write(string code, string comment, bool ident = true);

		/**
			Returns a string with the Hack register symbol related to a given segment.

			@param segment String with the name of the segment.

			@param index Index of the segment.

			@return String with the Hack (pre defined) symbol for the given segment.
		*/
		virtual string registerName(string segment,int index);

		/**
			Returns the full qualified symbol of a label. If the label is supplied during a
			function declaration, tne full qualified symbol will be "functionName$label".
			Otherwise it will be "moduleName.label".

			@param label String with label.

			@return String with full qualified symbol of the given label.
		*/
		virtual string labelSymbol(string label);

        ostream& outputStream; 		/**< The output stream where Hack assembly code will be written. */

		string programName;			/**< Name of the program. */

		string fileName; 			/**< Name of the VM being translated. */

		string moduleName; 			/**< Name of the current module (the @ref fileName without
									its extension). */

		string functionName;		/**< Name of the current function being translated. If no
									function is being translated, the string will be empty. */

		bool writeComments; 		/**< Flags the CodeWriter to output comments to the .asm file. */

		int callGlobalID; 			/**< Global ID for VM call command return label. */

		int eqModuleID; 			/**< Module ID for VM eq command labels. */

		int gtModuleID; 			/**< Module ID for VM gt command labels. */

		int ltModuleID; 			/**< Module ID for VM lt command labels. */

};

#endif // CODE_WRITER_INCLUDED_H
