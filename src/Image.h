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
		int type = 0;
		bool doIHaveTrans = true;
		string path = "";
		void bind();
		void load();
		void unload();
		int getType();
		bool isLoaded = false;
		int width;
		int height;
		int currentSlot = 0;
		unsigned int id = 0;
};
class SubImage : Image {
	public:
		SubImage();
		int type = 0;
		bool doIHaveTrans = true;
		string path = "";
		void bind();
		void load();
		void unload();
		int getType();
		bool isLoaded = false;
	private:
		int currentSlot = 0;
		unsigned int id = 0;
};