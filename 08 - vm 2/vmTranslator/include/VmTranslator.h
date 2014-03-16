#ifndef VM_TRANSLATOR_INCLUDED_H
#define VM_TRANSLATOR_INCLUDED_H

#include "CodeWriter.h"
#include "CodeWriterOpt.h"
#include "Parser.h"
#include "FileHandler.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

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

			@param unusedFunctions
		*/
		VmTranslator(string input, ostream& outputStream, bool verbose, bool bootstrap, bool comments, bool optimise,
               bool unusedFunctions);

		/**
			Translates the VM files from the input to the output stream.
		*/
		void translate();

		/**

		*/
		void printCalls();

		/**
			@return
		*/
		int numSkippedFunctions();

		/**

		*/
		void printSkippedFunctions();

	private:

		void translateVmCommand(Parser& parser, CodeWriter* codeWriter);

		/**
			Translates the VM files generating code without optimisation. This translation process closely
			follows the implementation design proposed by chapter 8 of the book and uses an instance of
			CodeWriter.

			@param parser Parser handling the current file being translated.

			@param codeWriter CodeWriter that is outputting Hack assembly code.

			@return ?????????
		*/
		//int translateNonOptimised(Parser& parser, CodeWriter* codeWriter);
		int translateNonOptimised();

		/**
			Translates the VM files generating code with optimisation. This is a departure from the book's
			proposed implementation  but yields .asm files with fewer instructions. Uses an instance of
			CodeWriterOpt.

			@param parser Parser handling the current file being translated.

			@param codeWriterOpt CodeWriterOpt that is outputting Hack assembly code.

			@return ?????
		*/
		//void translateOptimised(Parser& parser, CodeWriterOpt* codeWriterOpt);
		int translateOptimised();

        /**
			Tries to optimise the code. If a certain sequence of VM commands can be translated into
			optimised code, this method will write to the output stream the optimised Hack assembly
			code. The method determines a possible optimisation sequence by looking at the current
			and upcoming (looking ahead) VM commands.

			@param parser Parser handling the current file being translated.

			@param codeWriterOpt CodeWriterOpt that is outputting Hack assembly code.

        	@return True if a optimised sequence was written to the output stream. False otherwise.
        */
        bool translateOptimisedSequence(Parser& parser, CodeWriterOpt* codeWriterOpt);

        /**
			Updates the parser and look ahead vectors.

        	@param parser Parser handling the current file being translated.

        	@param skipVectors Number of entries to skip.

        	@param skipParser Number of calls to Parser.advance().
        */
        void updateLookAhead(Parser& parser, int skipVectores, int skipParser);

		/**
			Prints information related to the translation process.
		*/
		void printHeader();

		/**
		*/
		void scanFunctionCalls(string module, string function);

		/**

		*/
		bool advanceParser(Parser& parser, int num = 1);


		bool newFileHasStarted;

		string input; 						/**< Name of the input file or directory. */

		ostream& outputStream;				/**< Output stream in which the translator writes Hack assembly
											code. */

		FileHandler fileHandler;			/**< Handles the @ref input (being it a file or directory). */

		vector<VmCommandOptType> vmCmdVec;	/**< Vector of VmCommandOptType for look ahead. This vector
											has all the VM commands of the current file being translated. */

		vector<int> vmArg2Vec;				/**< Vector of second arguments for look ahead. The vector size
											is equal to @ref vmCmdList size. This vector has all the second
											arguments of the file being translated, with standard entry 0 for
											VM commands that do not have a second argument. */

		map<string, set<string> > funcCallMap;	/**< */

		vector<string> skippedFuncVec;

		bool verbose;						/**< Flags the verbose mode. */

		bool bootstrap; 					/**< Flags the VmTranslator to output bootstrap code.  */

		bool comments; 						/**< Flags the VmTranslator to include comments in the output. */

		bool optimise; 						/**< Flags the VmTranslator to optimise the code. */

        bool unusedFunctions;               /**< */

		const string vmTransTitle; 			/**< VmTranslator title. */

		const string vmTransSubtitle; 		/**< VmTranslator subtitle. */

		const string vmTransVersion;		/**< VmTranslator version. */

};

#endif // VM_TRANSLATOR_INCLUDED_H
