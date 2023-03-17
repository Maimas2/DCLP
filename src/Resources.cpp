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

namespace res {
	Image IMAGE_LIST;
	Image setupImage(string path, bool hasTrans) {
		Image i;
		if(isLowRes && path.starts_with("card-resources")) {
			i = loadImage(pathPrefix + "low-res/" +  path, hasTrans, IMAGE_ALWAYS_LOADED);
		} else {
			i = loadImage(pathPrefix + path, hasTrans, IMAGE_ALWAYS_LOADED);
		}
		return i;
	}
	void initial() {
		allWhite  = setupImage("images/white.png",                    false);

		baseCard  = setupImage("card-resources/CardColorOne.png",     true);
	baseOutline   = setupImage("card-resources/CardBrown.png",        true);
desciptionFocus   = setupImage("card-resources/DescriptionFocus.png", true);
		coin      = setupImage("card-resources/Coin.png",             true);
		debt      = setupImage("card-resources/Debt.png",             true);
		vpToken   = setupImage("card-resources/VP.png",               true);
		potion    = setupImage("card-resources/Potion.png",           true);
		cardGrey  = setupImage("card-resources/CardGray.png",         true);
		cardBase  = setupImage("card-resources/BaseCardGray.png",     true);
		traveler  = setupImage("card-resources/Traveller.png",        true);
		
		baseCardOutline = setupImage("card-resources/BaseCardGray.png",      true);
		baseCardColor   = setupImage("card-resources/BaseCardColorOne.png",  true);
		baseCardSide    = setupImage("card-resources/BaseCardBrown.png",     true);
		
		secondaryCard = setupImage("card-resources/CardColorTwo.png",        true);
		secondaryNight= setupImage("card-resources/CardColorTwoNight.png",   true);
		
		eventOutline  = setupImage("card-resources/EventBrown.png",          true);
		eventSide     = setupImage("card-resources/EventBrown2.png",         true);
		eventBase     = setupImage("card-resources/EventColorOne.png",       true);
		eventSecondary= setupImage("card-resources/EventColorTwo.png",       true);

		pileMarker= setupImage("card-resources/PileMarkerGrey.png",          true);
pileMarkerColored = setupImage("card-resources/PileMarkerColorOne.png",      true);

		matTop    = setupImage("card-resources/MatBannerTop.png",            true);
		matBottom = setupImage("card-resources/MatBannerBottom.png",         true);

		heirloom  = setupImage("card-resources/Heirloom.png", 			   true);

		traitBase = setupImage("card-resources/TraitBrown.png", 			   true);
	}
}
