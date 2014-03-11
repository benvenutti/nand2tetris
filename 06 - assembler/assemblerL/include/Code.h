#ifndef CODE_INCLUDED_H
#define CODE_INCLUDED_H

#include <string>

using namespace std;

class Code
{

	public:

		/**
			Returns the binary code of the dest mnemonic.

			@param mnemonic String with mnemonic.

			@return An unsigned integer with the dest mnemonic binary code.
		*/
		unsigned int dest(string mnemonic);

		/**
			Returns the binary code of the comp mnemonic.

			@param mnemonic String with mnemonic.

			@return An unsigned integer with the comp mnemonic binary code.
		*/
		unsigned int comp(string mnemonic);

		/**
			Returns the binary code of the jump mnemonic.

			@param mnemonic String with mnemonic.

			@return An unsigned integer with the jump mnemonic binary code.
		*/
		unsigned int jump(string mnemonic);

};

#endif // CODE_INCLUDED_H
