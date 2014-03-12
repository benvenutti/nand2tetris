#include "FileHandler.h"

FileHandler::FileHandler(string input)
	: input(input)
{
	files = vector<string>();

	if (isFile() && isJackFile()) {
		path = "";
		files.push_back(string(input));
	}
	else if (isDir()) {

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
	    	if (isJackFile(str))
	        	files.push_back(string(path + str));
	    }
	    closedir(dp);
	}

	it = files.begin();
}

FileHandler::~FileHandler()
{
}

bool FileHandler::isFile()
{
	struct stat buf;
	stat(input.c_str(), &buf);
	return S_ISREG(buf.st_mode);
}

bool FileHandler::isFile(string in)
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

bool FileHandler::isDir(string in)
{
	struct stat buf;
	stat(in.c_str(), &buf);
	return S_ISDIR(buf.st_mode);
}

string FileHandler::getOutputFile(string in)
{
	string str = ".xml";
	in.erase(in.rfind(".jack"));
	in.append(str);

	return in;
}

int FileHandler::getNumJackFiles()
{
	return files.size();
}

bool FileHandler::isJackFile()
{
	return input.rfind(".jack") != string::npos ? true : false;
}

bool FileHandler::isJackFile(string in)
{
	return in.rfind(".jack") != string::npos ? true : false;
}

bool FileHandler::nextJackFile(string& filename)
{
	if (it == files.end())
		return false;

	filename = *it;
	++it;

	return true;
}
