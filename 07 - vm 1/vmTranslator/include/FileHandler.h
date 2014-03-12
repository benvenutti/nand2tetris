#ifndef FILE_HANDLER_INCLUDED_H
#define FILE_HANDLER_INCLUDED_H

#include <vector>
#include <string>

using namespace std;

class FileHandler
{

	public:

		/**
			Constructs a FileHandler object and binds it to the given input.

			@param input String with the name of a file or directory.
		*/
		FileHandler(string input);

		/**
			Is the object handling a file?

			@return True if the object is handling a file. False otherwise.
		*/
		bool isFile();

		/**
			Is the object handling a file with ".vm" extension?

			@return True if the object is handling a file with ".vm" extension. False otherwise.
		*/
		bool isVmFile();

		/**
			Is the object handling a directory?

			@return True if the object is handling a directory. False otherwise.
		*/
		bool isDir();

		/**
			Returns the number of Vm files being handled by the object. If the object is
			handling a single file, a call to this method will return the integer one. If
			the object is handling a directory, this method will return the number of
			files with the ".vm" extension in the directory.

			@return Integer with the number of Vm files being handled by the object.
		*/
		int getNumVmFiles();

		/**
			Returns a string with the name of the program. If the object is handling a
			file, the name of the program is the name of the file without its path and
			extension. If the object is handling a directory, the name of the program
			is the directory's name without its path.

			@return String with the name of the program.
		*/
		string getProgramName();

		/**
			Returns a string with the path and name of the output file. If the object is
			handling a file, the output file name is the name of the input file with the
			extension ".asm" in it. If the object is handling a directory, the output file
			name is the name of the directory with ".asm" appended to it.

			@return String containing the path (if any) and the name of the ouput
			file with ".asm" extension.
		*/
		string getOutputFile();

        /**
            Checks if there is at leats one more Vm file to handle. If so, writes the name of
            the file in the supplied string.

            @param fileName String that will receive the name of the next avaiable Vm file.
            If there are no more files, the string will remain untouched.

            @return True if there is at least one more Vm file. False otherwise.
        */
		bool nextVmFile(string& fileName);

        /**
			Is the given string the name of a regular file?

            @param in String with a name of a possible file.

			@return True if supplied string is the name of a regular a file. False otherwise.
		*/
		static bool isFile(string in);

        /**
			Is the given string the name of a file with ".vm" extension?

            @param in String with the name of a possible file.

			@return True if the supplied string is the name of a file with ".vm" extension.
			False otherwise.
		*/
		static bool isVmFile(string in);

        /**
			Is the given string the name of a directory?

            @param in String with a name of a possible directory.

			@return True if the supplied string is the name of a directory. False otherwise.
		*/
		static bool isDir(string in);

        /**
			Returns a string with the path and name of the output file. If the supplied
			string is the name of a file, the output file name is the name of the input
			file with the extension ".asm" in it. If the supplied string is the name of
			a directory, the output file name is the name of the directory with ".asm"
			appended to it.

            @param in String with the input name.

			@return String containing the path (if any) and the name of the ouput
			file with ".asm" extension.
		*/
		static string getOutputFile(string in);

		/**
			Returns a string with the name of the program. If the supplied string is
			the name of a regular file, the name of the program is the name of the file
			without its path and extension. If the supplied string is the name of a
			directory, the name of the program is the directory's name without its path.

            @param in String with the input name.

			@return String with the name of the program.
		*/
		static string getProgramName(string fileName);

        /**
            Removes the extension of a supplied string with file name.

            @param fileName String with the file name to have its extension removed.

            @return String with the given file name without its extension. If the supplied
            string originally has now extension, a simple copy of the supplied string
            will be returned.
        */
		static string removeExtension(string fileName);

        /**
            Removes the full path of a given string.

            @param fileName String with file or directory name.

            @return String with supplied file or directory name without any path information
            (relative or absolute).
        */
		static string removePath(string fileName);

        /**
            Changes the extension in a file name. If the supplied file name
            has no extension, the new extension will simply be appended to it.

            @param fileName String with the file name to reveice new extension.

            @param nexExtension String with the new extension.

            @return String with the file name cointaining the new extension.
        */
		static string changeExtension(string fileName, string newExtension);

	private:

		string input;                   /**< */

		string path;                    /**< */

		vector<string> files;           /**< */

		vector<string>::iterator it;    /**< Vector iterator for @ref files. */

};

#endif // FILE_HANDLER_INCLUDED_H
