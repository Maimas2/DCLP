//ONLY C++ STANDARD LIBRARIES INCLUDED HERE
#include <string>
#include <iostream>

#include "OSspecifics.h"

using namespace std;

#ifdef __linux__
#include <pwd.h>
#include <unistd.h>
#include <filesystem>

const string fileSeparator = "/";

string getHomeDirectory() {
	if (getenv("HOME") != "") {
		return string(getpwuid(getuid())->pw_dir) + "/";
	}
	return "~";
}
string getPathToExe() {
	return filesystem::canonical("/proc/self/exe");
}
bool directoryExists(string dir) {
	return filesystem::exists(dir);
}
void createDirectory(string dir) {
	system(("mkdir " + dir).c_str());
}
void deleteFile(string file) {
	system(("rm " + file).c_str());
}
#elif defined(_WIN32)
#include <string.h>
#include <string>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

const string fileSeparator = "\\";

string getHomeDirectory() {
	string homeDir = string(getenv("HOMEDRIVE"));
	string Hhmepath = string(getenv("HOMEPATH"));
	//homeDir = malloc(strlen((char*)homeDir)+strlen(Hhmepath)+1);
	homeDir += Hhmepath + "\\";
	return homeDir;
}
bool directoryExists(string dir) {
	if( _access( dir.c_str(), 0 ) == 0 ){

        struct stat status;
        stat( dir.c_str(), &status );

        return (status.st_mode & S_IFDIR) != 0;
    }
    return false;
}
void createDirectory(string dir) {
	system(("mkdir " + dir).c_str());
}
void deleteFile(string file) {
	system(("del " + file + " /s").c_str());
}
#endif