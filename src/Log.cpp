#include <iostream>
#include <fstream>
#include <string>
#include <GLFW/glfw3.h>
#include <chrono>
#include <ctime>
#include <typeinfo>

#include "utils.h"
#include "main.h"

using namespace std;

#define outFile (*f)

namespace Log {
	//begin log utils
	string totalOutput;
	bool hasInited = false;
	void addString(string toAdd) {
		cout << toAdd << endl;
	}
	string getTimestamp() {
		string t = to_string(glfwGetTime());
		return "[" + t + "]";
	}
	//End util functions
	int loggerInit() {
		auto t = chrono::system_clock::to_time_t(chrono::system_clock::now());
		hasInited = true;
		return 0;
	}
	void loggerExit() {
		
	}
	void debug(string out) {
		// addString("[    DEBUG    ] " + getTimestamp() + " " + out);
	}
	void log(string out) {
		// addString("[     LOG     ] " + getTimestamp() + " " + out);
	}
	void warning(string out) {
		addString("[   WARNING   ] " + getTimestamp() + " " + out);
	}
	void spicyWarning(string out) {
		addString("[SPICY WARNING] " + getTimestamp() + " " + out);
	}
	void error(string out) {
		addString("[    ERROR    ] " + getTimestamp() + " " + out);
	}
	void fatal(string out, bool shouldKill) {
		addString(string("\n[ FATAL ERROR ]: ") + string(shouldKill ? "" : "NOT ") + "KILLING. Full details should be below:\n\n" + out);
		if(shouldKill) {
			dclpExit();
		}
	}
	void flushFile() {
		
	}
}