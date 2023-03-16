#include <vector>
#include <iostream>
#include <stdlib.h>
#include <algorithm>

#include "Resources.h"
#include "Image.h"
#include "utils.h"
#include "Log.h"
#include "main.h"

using namespace std;

IMAGE_TYPE IMAGE_ALWAYS_LOADED = 0;
IMAGE_TYPE IMAGE_GENERIC_MENU  = 1;
IMAGE_TYPE IMAGE_PLAYING       = 2;
IMAGE_TYPE IMAGE_GAME_CHOOSER  = 3;

namespace res {
	vector<Image*> images;
	vector<IMAGE_TYPE> typesLoaded;
	int lastReservedSpot = 0;
	Image container, ball, allWhite, circle, baseCard, baseOutline, desciptionFocus, coin, debt, cardGrey, bg, cardBase, secondaryCard, secondaryNight, tempIcon, eventOutline, eventSide, eventBase, eventSecondary, vpToken, potion, baseCardOutline, baseCardColor, baseCardSide, traveler, tempExpansionIcon, pileMarker, pileMarkerColored, matTop, matBottom;
	bool isOfType(IMAGE_TYPE im, IMAGE_TYPE typee) {
		return typee == im;
	}
	Image setupImage(string path, bool hasTrans, IMAGE_TYPE typee) {
		Image i;
		if(isLowRes && path.starts_with("card-resources")) {
			i = loadImage(pathPrefix + "low-res/" +  path, hasTrans, typee);
		} else {
			i = loadImage(pathPrefix + path, hasTrans, typee);
		}
		return i;
	}
	void initial() {
		allWhite  = setupImage("images/white.png",                    false,IMAGE_ALWAYS_LOADED);

		baseCard  = setupImage("card-resources/CardColorOne.png",     true, IMAGE_ALWAYS_LOADED);
	baseOutline   = setupImage("card-resources/CardBrown.png",        true, IMAGE_ALWAYS_LOADED);
desciptionFocus   = setupImage("card-resources/DescriptionFocus.png", true, IMAGE_ALWAYS_LOADED);
		coin      = setupImage("card-resources/Coin.png",             true, IMAGE_ALWAYS_LOADED);
		debt      = setupImage("card-resources/Debt.png",             true, IMAGE_ALWAYS_LOADED);
		vpToken   = setupImage("card-resources/VP.png",               true, IMAGE_ALWAYS_LOADED);
		potion    = setupImage("card-resources/Potion.png",           true, IMAGE_ALWAYS_LOADED);
		cardGrey  = setupImage("card-resources/CardGray.png",         true, IMAGE_ALWAYS_LOADED);
		cardBase  = setupImage("card-resources/BaseCardGray.png",     true, IMAGE_ALWAYS_LOADED);
		traveler  = setupImage("card-resources/Traveller.png",        true, IMAGE_ALWAYS_LOADED);
		
		baseCardOutline = setupImage("card-resources/BaseCardGray.png",      true, IMAGE_ALWAYS_LOADED);
		baseCardColor   = setupImage("card-resources/BaseCardColorOne.png",  true, IMAGE_ALWAYS_LOADED);
		baseCardSide    = setupImage("card-resources/BaseCardBrown.png",     true, IMAGE_ALWAYS_LOADED);
		
		secondaryCard = setupImage("card-resources/CardColorTwo.png",        true, IMAGE_ALWAYS_LOADED);
		secondaryNight= setupImage("card-resources/CardColorTwoNight.png",   true, IMAGE_ALWAYS_LOADED);
		
		eventOutline  = setupImage("card-resources/EventBrown.png",          true, IMAGE_ALWAYS_LOADED);
		eventSide     = setupImage("card-resources/EventBrown2.png",         true, IMAGE_ALWAYS_LOADED);
		eventBase     = setupImage("card-resources/EventColorOne.png",       true, IMAGE_ALWAYS_LOADED);
		eventSecondary= setupImage("card-resources/EventColorTwo.png",       true, IMAGE_ALWAYS_LOADED);

		pileMarker= setupImage("card-resources/PileMarkerGrey.png",          true, IMAGE_ALWAYS_LOADED);
pileMarkerColored = setupImage("card-resources/PileMarkerColorOne.png",      true,IMAGE_ALWAYS_LOADED);

		matTop    = setupImage("card-resources/MatBannerTop.png",            true, IMAGE_ALWAYS_LOADED);
		matBottom = setupImage("card-resources/MatBannerBottom.png",         true, IMAGE_ALWAYS_LOADED);
		
		typesLoaded.push_back(IMAGE_ALWAYS_LOADED);
	}
	void unloadAll() {
		for(int i = 0; i < images.size(); i++) {
			images[i]->unload();
		}
		typesLoaded.clear();
		typesLoaded.push_back(IMAGE_ALWAYS_LOADED);
	}
	void unload(IMAGE_TYPE typee) {
		Log::log("Unloading image type " + to_string(typee));
		for(int i = 0; i < NUMBER_OF_IMAGE_SLOTS; i++) {
			if(images[i]->type == typee) images[i]->unload();
		}
		auto finded = find(typesLoaded.begin(), typesLoaded.end(), typee);
		if(finded != typesLoaded.end()){
			typesLoaded.erase(finded);
		}
	}
	void load(IMAGE_TYPE typee) {
		Log::log("Loading image type " + to_string(typee));
		/*for(int i = 0; i < NUMBER_OF_IMAGE_SLOTS; i++) {
			if(images[i]->type == typee) images[i]->load();
		}*/
		for(int i = 0; i < images.size(); i++) {
			if(images[i]->type == typee) images[i]->load();
		}
		typesLoaded.push_back(typee);
	}
	void unloadAndLoad(IMAGE_TYPE typee) {
		unloadAll();
		load(typee);
	}
}
