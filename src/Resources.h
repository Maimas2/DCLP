#pragma once
#include <vector>

#include "Image.h"
#include "utils.h"

using namespace std;

typedef int IMAGE_TYPE;

extern IMAGE_TYPE IMAGE_ALWAYS_LOADED;
extern IMAGE_TYPE IMAGE_GENERIC_MENU;
extern IMAGE_TYPE IMAGE_PLAYING;
extern IMAGE_TYPE IMAGE_GAME_CHOOSER;

#define NUMBER_OF_IMAGE_SLOTS images.size()

#define IMAGE_LIST container, ball, allWhite, circle, baseCard, baseOutline, desciptionFocus, coin, debt, cardGrey, bg, cardBase, \
secondaryCard, secondaryNight, tempIcon, eventOutline, eventSide, eventBase, eventSecondary, vpToken, potion, baseCardOutline, \
baseCardColor, baseCardSide, traveler, tempExpansionIcon, pileMarker, pileMarkerColored, matTop, matBottom, heirloom, traitBase, \
traitSide, traitColor, traitColorSide

namespace res {
	extern Image IMAGE_LIST;
	void initial();
	Image setupImage(string path, bool hasTrans);
}
