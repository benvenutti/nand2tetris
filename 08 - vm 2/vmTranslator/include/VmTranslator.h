#ifndef VM_TRANSLATOR_INCLUDED_H
#define VM_TRANSLATOR_INCLUDED_H

#include "CodeWriter.h"
#include "CodeWriterOptimised.h"
#include "Parser.h"
#include "FileHandler.h"
#include <iostream>
#include <string>
#include <vector>

class VmTranslator
{

	public:

		/**
			Constructs a VmTranslator object and binds it to the given input file/directory
			and output stream.

			@note Both input and outputStream parameters must be valid. Otherwise the translation
			may cause unexpected behavior and/or erratic results.

			@param input Name of a valid VM file or directory with VM files.

			@param outputStream Output stream in which the translator will write Hack assembly code.

			@param verbose Flags the VmTranslator to switch verbose mode on or off.

			@param bootstrap Flags the VmTranslator to write the bootstrap code.

			@param comments Flags the VmTranslator to write comments (VM commands and others) to the
			output stream.

			@param optimise Flags the VmTranslator to optimise the generated code.
		*/
		VmTranslator(string input, ostream& outputStream, bool verbose, bool bootstrap, bool comments, bool optimise);

		/**
			Translates the VM files from the input to the output stream.
		*/
		void translate();

	private:

		/**
			Translates the VM files generating code without optimisation. This translation process closely
			follows the implementation design proposed by chapter 8 of the book and uses an instance of
			CodeWriter.
		*/
		void translateNonOptimised(Parser& parser, CodeWriter* codeWriter);

		/**
			Translates the VM files generating code with optimisation. This is a departure from the proposed
			implementation but yields .asm files with fewer instructions. Uses an instance of
			CodeWriterOptimised.

		*/
		void translateOptimised(Parser& parser, CodeWriterOptimised* codeWriterOpt);

        /**
        */
        bool translateOptimisedSequence(Parser& parser, CodeWriterOptimised* codeWriterOpt);

		/**
			Prints information related to the translation process.
		*/
		void printHeader();

        /**
        */
        class VmCommand {
            public:
                VmCommand(VmCommandType cmd, string arg1, int arg2)
                    : cmd(cmd), arg1(arg1), arg2(arg2)
                {}

                VmCommandType cmd;
                string arg1;
                int arg2;
        };

		string input; 					/**< Name of the input file or directory. */

		ostream& outputStream;			/**< Output stream in which the translator writes Hack assembly code.  */

		FileHandler fileHandler;		/**< Handles the @ref input (being it a file or directory). */

		vector<VmCommandOptType> vmCmdList;  /**< */

		bool verbose;					/**< Flags the verbose mode. */

		bool bootstrap; 				/**< Flags the VmTranslator to output bootstrap code.  */

		bool comments; 					/**< Flags the VmTranslator to include comments in the output. */

		bool optimise; 					/**< Flags the VmTranslator to optimise the code. */

		const string vmTransTitle; 		/**< VmTranslator title. */

		const string vmTransSubtitle; 	/**< VmTranslator subtitle. */

		const string vmTransVersion;	/**< VmTranslator version. */

};

#endif // VM_TRANSLATOR_INCLUDED_H
