#include <string>

using namespace std;

namespace Log {
	extern string totalOutput;
	int loggerInit();
	void loggerExit();
	void debug(string out);
	void log(string out);
	void warning(string out);
	void spicyWarning(string out);
	void error(string out);
	void fatal(string out, bool shouldKill);
	void flushFile();
}