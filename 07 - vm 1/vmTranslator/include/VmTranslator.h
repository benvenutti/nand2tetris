#ifndef VM_TRANSLATOR_INCLUDED_H
#define VM_TRANSLATOR_INCLUDED_H

#include "FileHandler.h"
#include <iostream>
#include <string>

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
		*/
		VmTranslator(string input, ostream& outputStream, bool verbose);

		/**
			Translates the VM files from the input to the output stream.
		*/
		void translate();

	private:

		/**
			Prints information related to the translation process.
		*/
		void printHeader();

		string input; 					/**< Name of the input file or directory. */

		ostream& outputStream;			/**< Output stream in which the translator writes Hack assembly code.  */

		FileHandler fileHandler;		/**< Handles the @ref input being it a file or directory. */

		bool verbose;					/**< Flag for the verbose mode. */

		const string vmTransTitle; 		/**< VmTranslator title. */

		const string vmTransSubtitle; 	/**< VmTranslator subtitle. */

		const string vmTransVersion;	/**< VmTranslator version. */

};

#endif // VM_TRANSLATOR_INCLUDED_H
