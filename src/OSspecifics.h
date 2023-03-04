#include <string>

using namespace std;

extern const string fileSeparator;

string getHomeDirectory();
string getPathToExe();
bool directoryExists(string dir);
void createDirectory(string dir);
void deleteFile(string file);