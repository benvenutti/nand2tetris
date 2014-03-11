#ifndef FILE_HANDLER_INCLUDED_H
#define FILE_HANDLER_INCLUDED_H

#include <string>

using namespace std;

namespace FileHandler
{

	/**
		Changes the extension in a file name. If the supplied file name
		has no extension, the new extension will simply be appended to it.

		@param fileName String with the file name to reveice new extension.

		@param nexExtension String with the new extension.

		@return String with the file name cointaining the new extension.
	*/
	string changeExtension(string fileName, string newExtension);

	/**
		Does the supplied file name has the supplied extension?

		@param fileName String with the name of the file to be checked.

		@param extension String with the extension to be checked.

		@return True is file has supplied extension. False otherwise.
	*/
	 bool hasExtension(string fileName, string extension);

	/**
		Is the file name pointing to a file?

		@param String with the name of the file to be checked.

		@return True if it's a file. False otherwise.
	*/
	 bool isFile(string fileName);

};

#endif // FILE_HANDLER_INCLUDED_H
