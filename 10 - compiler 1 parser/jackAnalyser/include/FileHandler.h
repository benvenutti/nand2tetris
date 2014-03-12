#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

class FileHandler
{

	public:

		FileHandler(string input);

		~FileHandler();

		bool isFile();

		bool isFile(string in);

		bool isDir();

		bool isDir(string in);

		int getNumJackFiles();

		string getOutputFile(string in);

		bool isJackFile();

		bool isJackFile(string in);

		bool nextJackFile(string& filename);

	private:

		string input;

		string path;

		vector<string> files;

		vector<string>::iterator it;

};

#endif // FILE_HANDLER_H
