#include "FileHandler.h"
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>

FileHandler::FileHandler(string input)
	: input(input)
{
	if (isFile() && isVmFile()) {

		path = "";
		files.push_back(string(input));

	} else if (isDir()) {

		if (input.back() == '/')
			path = input;
		else
			path = input + "/";

		DIR *dp;
	    struct dirent *dirp;
	    if((dp  = opendir(input.c_str())) == NULL) {
	        cerr << "Error opening " << input << endl;
	    }

	    while ((dirp = readdir(dp)) != NULL) {
	    	string str(dirp->d_name);
	    	if (str == "." || str == "..")
	    		continue;
	    	if (FileHandler::isVmFile(str))
	        	files.push_back(string(path + str));
	    }
	    closedir(dp);
	}

	it = files.begin();
}

bool FileHandler::isFile()
{
	struct stat buf;
	stat(input.c_str(), &buf);
	return S_ISREG(buf.st_mode);
}

bool FileHandler::isFile(string in) // static
{
	struct stat buf;
	stat(in.c_str(), &buf);
	return S_ISREG(buf.st_mode);
}

bool FileHandler::isDir()
{
	struct stat buf;
	stat(input.c_str(), &buf);
	return S_ISDIR(buf.st_mode);
}

bool FileHandler::isDir(string in) // static
{
	struct stat buf;
	stat(in.c_str(), &buf);
	return S_ISDIR(buf.st_mode);
}

string FileHandler::getOutputFile()
{
	string in = input;

	if (isDir()) {
		return path + getProgramName().append(".asm");
	}

	string ext = ".asm";
	if (in.rfind(".vm") != string::npos)
		in.erase(in.rfind(".vm"));
	in.append(ext);

	return in;
}

string FileHandler::getOutputFile(string in) // static
{
	struct stat buf;
	stat(in.c_str(), &buf);
	if (S_ISDIR(buf.st_mode)) {
		string path;
		if (in.back() == '/')
			path = in;
		else
			path = in + "/";

		return path + FileHandler::getProgramName(in).append(".asm");
	}

	string ext = ".asm";
	if (in.rfind(".vm") != string::npos)
		in.erase(in.rfind(".vm"));
	in.append(ext);

	return in;
}

string FileHandler::removeExtension(string fileName) // static
{
	if (fileName.rfind(".") != string::npos)
		fileName.erase(fileName.rfind("."));

	return fileName;
}

string FileHandler::getProgramName(string fileName) // static
{
	fileName = removeExtension(fileName);
	return removePath(fileName);
}

string FileHandler::changeExtension(string fileName, string newExtension) // static
	{
		if (fileName.rfind(".") != string::npos)
			fileName.erase(fileName.rfind("."));

		fileName.append(newExtension);

		return fileName;
	}

string FileHandler::getProgramName()
{
	string progName;

	if (isVmFile()) {
		progName = input;
		progName.erase(progName.rfind(".vm"));
	} else if (input == ".") {
		char cwd[256];
		getcwd(cwd, sizeof(cwd));
		progName = cwd;
	} else {
		progName = input;
	}

	if (progName.back() == '/')
		progName.erase(progName.rfind("/"));

	if (progName.rfind("/") != string::npos)
		progName.erase(0, progName.rfind("/") + 1);

	return progName;
}

string FileHandler::removePath(string fileName) // static
{
	if (fileName.back() == '/')
		fileName.erase(fileName.rfind("/"));

	if (fileName.rfind("/") != string::npos)
		fileName.erase(0, fileName.rfind("/") + 1);

	return fileName;
}

int FileHandler::getNumVmFiles()
{
	return files.size();
}

bool FileHandler::isVmFile()
{
	return input.rfind(".vm") != string::npos ? true : false;
}

bool FileHandler::isVmFile(string in)
{
	return in.rfind(".vm") != string::npos ? true : false;
}

bool FileHandler::nextVmFile(string& fileName)
{
	if (it == files.end())
		return false;

	fileName = *it;
	++it;

	return true;
}
