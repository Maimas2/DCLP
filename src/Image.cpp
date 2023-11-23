#include "Image.h"
#include <glad/glad.h>
#include <iostream>

#include "utils.h"
#include "Log.h"
#include "Resources.h"
#include "main.h"

//bool isLoaded = false;

using namespace std;

/*class Image {
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
	private:
		int currentSlot = 0;
		unsigned int id = 0;
};*/

Image::Image() {
	path = "";
}

void Image::bind() {
	if(id == 0) {
		load();
	}
	if(id != 0) {
		glBindTexture(GL_TEXTURE_2D, id);
		currentlyBindedImage = this;
	}
}

void Image::load() {
	if(path.size() > 10000) {
		Log::warning("Image has length of " + to_string(path.size()) + "! Not trying to load.");
		return; // Odd bug where strings (uninitialized, i guess) have lengths in the tens of trillions, throwing std::bad_alloc
	}
	ImageStruct img = loadImageToInt(path, doIHaveTrans);
	if(img.textureId == 0) {
		Log::warning("Image failed to load!");
		return;
	}
	width = img.width;
	height= img.height;
	id = img.textureId;
	Log::log("Loading image " + path);
}