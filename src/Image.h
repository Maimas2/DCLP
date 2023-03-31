#pragma once
#include <string>
#include <vector>

using namespace std;

/*
 * IMAGE_ALWAYS_LOADED = 0
 * IMAGE_GENERIC_MENU = 1
 * IMAGE_PLAYING = 2
 */

class Image {
	public:
		Image();
		bool doIHaveTrans = true;
		string path = "";
		void bind();
		void load();
		int width;
		int height;
		unsigned int id = 0;
};