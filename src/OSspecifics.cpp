//ONLY C++ STANDARD LIBRARIES INCLUDED HERE
#include <string>
#include <iostream>
#include <cstring>

#include "OSspecifics.h"

using namespace std;

#ifdef __linux__
#include <pwd.h>
#include <unistd.h>
#include <filesystem>

const string fileSeparator = "/";

string getHomeDirectory() {
	if (strcmp(getenv("HOME"), "") != 0) {
		return string(getpwuid(getuid())->pw_dir) + "/";
	}
	return "~";
}
string getPathToExe() {
	return filesystem::canonical("/proc/self/exe");
}
string getPathToPwd() {
	return filesystem::current_path();
}
bool directoryExists(string dir) {
	return filesystem::exists(dir);
}
void createDirectory(string dir) {
	filesystem::create_directories(dir);
}
void deleteFile(string file) {
	filesystem::remove(file);
}
int forkNew() {
	int pid = fork();
	if(pid == 0) {
		execv("./DCLP", (char *const[]){(char*)"./DCLP"});
		return 0;
	} else {
		return pid;
	}
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
	homeDir += Hhmepath + "\\";
	return homeDir;
}
string getPathToPwd() {
	return filesystem::canonical("/proc/self/exe");
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