#pragma once
#include <vector>
#include <string>

#include "Image.h"

using namespace std;

#define NUMBER_OF_IMAGE_SLOTS images.size()

#define IMAGE_LIST container, ball, allWhite, circle, baseCard, baseOutline, desciptionFocus, coin, debt, cardGrey, bg, cardBase, \
secondaryCard, secondaryNight, tempIcon, eventOutline, eventSide, eventBase, eventSecondary, vpToken, potion, baseCardOutline, \
baseCardColor, baseCardSide, traveler, tempExpansionIcon, pileMarker, pileMarkerColored, matTop, matBottom, heirloom, traitBase, \
traitSide, traitColor, traitColorSide, baseCardColorTwo

namespace res {
	extern Image IMAGE_LIST;
	void initial();
	Image setupImage(string path, bool hasTrans);
}
