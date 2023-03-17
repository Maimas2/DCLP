#include "Image.h"
#include <glad/glad.h>
#include <iostream>

#include "utils.h"
#include "Log.h"
#include "Resources.h"

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
   glBindTexture(GL_TEXTURE_2D, id);
}

void Image::load() {
	if(isLoaded) return;
	//id = loadImageToInt(path, doIHaveTrans).textureId;
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
	isLoaded = true;
	Log::log("Loading image " + path);
}

void Image::unload() {
	if(!isLoaded || type == IMAGE_ALWAYS_LOADED) return;
	Log::log("Unloading image " + to_string(id)); // Has to log id instead of path because it was causing weird segfaults.
	glDeleteTextures(1, &id);
	id = 0;
	isLoaded = false;
	//cout << path << endl;
}
int Image::getType() {
	return type;
}
void SubImage::bind() {

}
