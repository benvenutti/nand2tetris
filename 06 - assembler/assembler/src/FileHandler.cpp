#include "FileHandler.h"
#include <sys/stat.h>
#include <dirent.h>

namespace FileHandler {

	string changeExtension(string fileName, string newExtension)
	{
		if (fileName.rfind(".") != string::npos)
			fileName.erase(fileName.rfind("."));

		fileName.append(newExtension);

		return fileName;
	}

	bool hasExtension(string fileName, string extension)
	{
		return fileName.rfind(extension) != string::npos ? true : false;
	}

	bool isFile(string fileName)
	{
		struct stat buf;
		stat(fileName.c_str(), &buf);
		return S_ISREG(buf.st_mode);
	}

}
