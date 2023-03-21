#include <string>

using namespace std;

extern const string fileSeparator;

string getHomeDirectory();
string getPathToExe();
string getPathToPwd();
bool directoryExists(string dir);
void createDirectory(string dir);
void deleteFile(string file);
int forkNew();