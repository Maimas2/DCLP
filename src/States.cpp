#ifndef GL_IS_INCLUDED
#define GL_IS_INCLUDED
#include "glm/fwd.hpp"
#include <csetjmp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <type_traits>
#endif
#include <iostream>
#include <cmath>
#include <time.h>
#include <map>
#include <vector>
#include <regex>
#include <cstring>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include "States.h"
#include "handler.h"
#include "utils.h"
#include "Text.h"
#include "main.h"
#include "Resources.h"

#define isNight      (cardColor == 10 || cardSecondary == 11)
#define isTitleWhite ((cardColor == 10 && cardSecondary == 0) || cardSecondary == 11)
#define isTextWhite  (cardColor == 10)
#define isBaseTitleWhite (cardColor == 10)
#define isBaseTypeWhite  (cardSecondary == 11)

const char* colorChoices[] = {"Action/Event", "Treasure", "Victory", "Reaction", "Duration", "Reserve", "Curse", "Shelter", "Ruins", "Landmark", "Night", "Boon", "Hex", "State", "Artifact", "Project", "Way", "Ally"};

float* matColorPointers[] = {allBlack, allRed, allGreen, allBrown, allBlue};

using namespace std;

map<string, vector<float>> colors;

vector<vector<float>> co;

string s;
string sss;

float tempColor[12];
float embellishmentColor[12] = {
	1.f, 1.f, 1.f,
	1.f, 1.f, 1.f,
	1.f, 1.f, 1.f,
	1.f, 1.f, 1.f,
};
float sideColor[12] = {
	1.2f, 0.8f, 0.5f,
	1.2f, 0.8f, 0.5f,
	1.2f, 0.8f, 0.5f,
	1.2f, 0.8f, 0.5f,
};
float secondaryColor[12];

bool hasInited = false;
void MainDrawState::update() {
    if(!hasInited) {
		hasInited = true;
		
		co.push_back({1, 1, 1});
		co.push_back({1.1, 0.95, 0.55});
		co.push_back({0.75, 0.9, 0.65});
		co.push_back({0.65, 0.8, 1.05});
		co.push_back({1.2, 0.8, 0.4});
		co.push_back({0.9, 0.75, 0.5});
		co.push_back({0.85, 0.6, 1.1});
		co.push_back({1.05, 0.65, 0.5});
		co.push_back({0.75, 0.6, 0.35});
		co.push_back({0.45, 1.25, 0.85});
		co.push_back({0.3, 0.4, 0.45});
		co.push_back({1.4, 1.35, 0.55, 0, 0, 0, 1.7, 1.25, 0.65, 1.95, 1.6, 0.4});
		co.push_back({0.75, 0.6, 2.1, 0, 0, 0, 0.8, 0.8, 0.8, 1.0, 0.75, 2.1});
		co.push_back({1.1, 1.3, 1.3, 0.6, 0.15, 0, 1.55, 1.15, 1.05, 1.4, 0.65, 0.45});
		co.push_back({1, 0.662, 0.365, 0.3, 0.15, 0.05});
		co.push_back({1, 0.643, 0.565, 0.4, 0.2, 0.15});
		co.push_back({1, 1.15, 1.25, 0.25, 0.3, 0.35, 1.6, 1.6, 1.6, 1.3, 1.3, 1.3});
		co.push_back({1, 0.819, 0.455, 0.35, 0.3, 0.15, 0.9, 0.8, 0.7, 0.9, 0.8, 0.7});
		co.push_back({0.95, 0.8, 1.1, 0.3, 0.25, 0.35, 1.6, 1.6, 1.6, 1.3, 1.3, 1.3});
		
		for(int i = 0; i < co.size(); i++) {
			break;
			float ma = max(co[i][0], max(co[i][1], co[i][2]));
			
			if(ma > 1.f) {
				co[i][0] /= ma;
				co[i][1] /= ma;
				co[i][2] /= ma;
			}
		}
	}
}
void doColors() {
	if(cardColor < co.size()) {
		for(int i = 0; i < 12; i++) {
			tempColor[i] = co[cardColor].at(i % 3);
		}
		if(co[cardColor].size() >= 9) {
			for(int i = 0; i < 12; i++) {
				embellishmentColor[i] = co[cardColor].at((i % 3) + 6);
			}
		} else {
			for(int i = 0; i < 12; i++) {
				embellishmentColor[i] = 1.f;
			}
		}
		if(co[cardColor].size() == 12) {
			for(int i = 0; i < 12; i++) {
				sideColor[i] = co[cardColor].at((i % 3) + 9);
			}
		} else {
			for(int i = 0; i < 12; i++) {
				sideColor[i] = allBrown[i];
			}
		}
	} else if(cardColor == co.size()) {
		for(int i = 0; i < 12; i++) {
			tempColor[i] = customCardColor[i % 3];
		}
		for(int i = 0; i < 12; i++) {
			embellishmentColor[i] = 1.f;
		}
	} else {
		for(int i = 0; i < 12; i++) {
			tempColor[i] = customCardColor[i % 3];
		}
		for(int i = 0; i < 12; i++) {
			embellishmentColor[i] = customEmbellishmentColor[i % 3];
		}
		for(int i = 0; i < 12; i++) {
			sideColor[i] = customSideColor[i % 3];
		}
	}
	if(cardSecondary == co.size()+1) {
		for(int i = 0; i < 12; i++) {
			secondaryColor[i] = secondCustomCardColor[i % 3];
		}
	} else {
		if(cardSecondary != 0) {
			for(int i = 0; i < 12; i++) {
				secondaryColor[i] = co[max(0, cardSecondary-1)][i % 3];
			}
		} else {
			for(int i = 0; i < 12; i++) {
				secondaryColor[i] = tempColor[i % 3];
			}
		}
	}
}
void drawBase() {
	if(cardLayout == 0) {
		setVec2("maxNE", 0.55f, 0.75f);
		setVec2("maxSW", -0.55f, -0.05f);
		
		res::tempIcon.bind();
		float wxh = (float)res::tempIcon.width / (float)res::tempIcon.height, wid, height;
		if(wxh < 1.f) {
			wid    = 1.1f;
			height = 1.1f/wxh;
		} else {
			wid    = 0.8f*wxh;
			height = 0.8f;
		}
		setBool("maxX", true);
		drawTexturedQuad((-wid/2+(xMove*0.55f))*zoom, 0.35f - ((height/2)*zoom - yMove), wid*zoom, height*zoom);
		setBool("maxX", false);
		if(cardSecondary == 0) {
			res::baseCard.bind();
			
			drawColoredTexture(-0.6522547652254765, -1.f, 2.f, 0.6522547652254765*2, 2.f, tempColor);
			
			if(!isNight) {
				res::desciptionFocus.bind();
				
				drawTexturedQuad(-(0.589532028+0.6522547652254765)/2, -0.017201302-0.393305439*2, 2.f, 0.589532028+0.6522547652254765, 0.393305439*2);
			}
		} else {
			res::baseCard.bind();
			
			drawColoredTexture(-0.6522547652254765, -1.f, 2.f, 0.6522547652254765*2, 2.f, tempColor);
			
			if(isNight) {
				res::secondaryNight.bind();
			} else {
				res::secondaryCard.bind();
			}
			
			if(isScreenshotting) {
				for(int i = 0; i < 12; i++) {
					secondaryColor[i] = co[cardSecondary-1][i % 3];
				}
			}
			
			drawColoredTexture(-0.6522547652254765, -1.f, 2.f, 0.6522547652254765*2, 2.f, secondaryColor);
		}
		if(isTraveler) {
			res::traveler.bind();
			drawTexturedQuad(-0.17f, -0.67f, 0.34f, 0.54f);
		}
	} else if(cardLayout == 1) {
		setVec2("maxNE", 0.9f, 0.448f);
		setVec2("maxSW", -0.9f, -0.24f);
		
		res::tempIcon.bind();
		float wxh = (float)res::tempIcon.width / (float)res::tempIcon.height, wid, height;
		if(wxh < 1.f) {
			wid    = 1.8f;
			height = 1.8f/wxh;
		} else {
			wid    = 0.75f*wxh;
			height = 0.75f;
		}
		setBool("maxX", true);
		drawTexturedQuad((-wid/2+(xMove*0.55f))*zoom, 0.104-(height/2+(yMove*0.4f))*zoom, wid*zoom, height*zoom);
		setBool("maxX", false);

		if(isTrait) {
			res::traitColor.bind();
				
			drawColoredTexture(-1.f, -0.6522547652254765, 2.f, 0.6522547652254765*2, tempColor);
		} else {
			if(cardSecondary == 0) {
				res::eventBase.bind();
				
				drawColoredTexture(-1.f, -0.6522547652254765, 2.f, 0.6522547652254765*2, tempColor);
			} else {
				res::eventBase.bind();
				
				drawColoredTexture(-1.f, -0.6522547652254765, 2.f, 0.6522547652254765*2, tempColor);
				
				res::eventSecondary.bind();
				
				drawColoredTexture(-1.f, -0.6522547652254765, 2.f, 0.6522547652254765*2, secondaryColor);
			}
		}
	} else if(cardLayout == 2) {
		setVec2("maxNE", 0.55f, 0.75f);
		setVec2("maxSW", -0.55f, -0.75f);
		
		res::tempIcon.bind();
		float wxh = (float)res::tempIcon.width / (float)res::tempIcon.height, wid, height;
		if(wxh < 1.f) {
			wid    = 1.1f;
			height = 1.1f/wxh;
		} else {
			wid    = 0.8f*wxh;
			height = 0.8f;
		}
		setBool("maxX", true);
		drawTexturedQuad((-wid/2+(xMove*0.55f))*zoom, (-height/2+(yMove*1.5f))*zoom, wid*zoom, height*zoom);
		setBool("maxX", false);
		
		res::baseCardColor.bind();
		
		drawColoredTexture(-0.6522547652254765, -1.f, 2.f, 0.6522547652254765*2, 2.f, tempColor);

		res::baseCardColorTwo.bind();
		
		drawColoredTexture(-0.6522547652254765, -1.f, 2.f, 0.6522547652254765*2, 2.f, secondaryColor);
		
		res::baseCardOutline.bind();
		
		drawTexturedQuad(-0.6522547652254765, -1.f, 2.f, 0.6522547652254765*2, 2.f);
		
		res::baseCardSide.bind();
		
		drawColoredTexture(-0.6522547652254765, -1.f, 2.f, 0.6522547652254765*2, 2.f, sideColor);
	} else if(cardLayout == 3) {
		setVec2("maxNE", 0.8f, 0.5f);
		setVec2("maxSW", -0.8f, -0.5f);
		res::tempIcon.bind();
		float wxh = (float)res::tempIcon.width / (float)res::tempIcon.height, wid, height;
		if(wxh < 1.f) {
			wid    = 1.1f;
			height = 1.1f/wxh;
		} else {
			wid    = 0.8f*wxh;
			height = 0.8f;
		}
		setBool("maxX", true);
		drawTexturedQuad((-wid/2+(xMove*0.55f))*zoom, (-height/2+(yMove*1.5f))*zoom, wid*zoom, height*zoom);
		setBool("maxX", false);

		res::pileMarker.bind();
		drawColoredTexture(-1.f, -0.6522547652254765, 2.f, 0.6522547652254765*2, embellishmentColor);

		res::pileMarkerColored.bind();
		drawColoredTexture(-1.f, -0.6522547652254765, 2.f, 0.6522547652254765*2, tempColor);
	} else if(cardLayout == 4) {
		setVec2("maxNE", 0.99f, 0.717068966f);
		setVec2("maxSW", -0.99f, -0.717068966f);

		drawColoredQuad(-1.f, -0.727068966f, 2.f, 0.727068966f*2, matColorPointers[matColor]);
		if(hasImage) {
			res::tempIcon.bind();
			float wxh = (float)res::tempIcon.width / (float)res::tempIcon.height, wid, height;
			if(wxh < 1.f) {
				wid    = 1.1f;
				height = 1.1f/wxh;
			} else {
				wid    = 0.8f*wxh;
				height = 0.8f;
			}
			setBool("maxX", true);
			zoom *= 1.78;
			drawTexturedQuad((-wid/2+(xMove*0.55f))*zoom, (-height/2+(yMove*1.5f))*zoom, wid*zoom, height*zoom);
			zoom /= 1.78;
			setBool("maxX", false);
		}

		res::matTop.bind();
		drawColoredTexture(-1.f, -0.737068966, 2.f, 0.737068966*2, allWhite);

		res::matBottom.bind();
		drawColoredTexture(-1.f, -0.737068966, 2.f, 0.737068966*2, allWhite);
	}
}
void drawEmbellishments() {
	if(cardLayout == 0) {
		res::cardBase.bind();
		drawTexturedQuad(-0.6522547652254765, -1.f, 2.f, 0.6522547652254765*2, 2.f);

		res::cardGrey.bind();
		drawColoredTexture(-0.6522547652254765, -1.f, 2.f, 0.6522547652254765*2, 2.f, embellishmentColor);
		
		res::baseOutline.bind();
		drawColoredTexture(-0.6522547652254765, -1.f, 2.f, 0.6522547652254765*2, 2.f, sideColor);

		if(strcmp(heirloomText, "") != 0) {
			res::heirloom.bind();
			drawTexturedQuad(-0.576476058, -0.72f, 2.f, 0.581476058*2, 0.134821013);
			setFont("tnri");
			drawCenteredStringWithMaxWidth(string(heirloomText), 0.f, -0.666f, 1.25f, 0.9f);
			setFont("trajan");
		}
	} else if(cardLayout == 1) {
		if(isTrait) {
			res::traitBase.bind();
			drawColoredTexture(-1.f, -0.6522547652254765, 2.f, 0.6522547652254765*2, embellishmentColor);

			res::traitColorSide.bind();
			drawColoredTexture(-1.f, -0.6522547652254765, 2.f, 0.6522547652254765*2, tempColor);

			res::traitSide.bind();
			drawColoredTexture(-1.f, -0.6522547652254765, 2.f, 0.6522547652254765*2, eventColor);

			float cr = (isTitleWhite ? 1.f : 0.f);
			float cg = (isTitleWhite ? 1.f : 0.f);
			float cb = (isTitleWhite ? 1.f : 0.f);

			drawCenteredStringWithMaxWidth("Trait", 0.f, 0.51f, 2.f, 0.66f, cr, cg, cb);

			setMat4("transMat", glm::rotate(1.57079633f, glm::vec3(0.f, 0.f, 1.f)));
			drawCenteredStringWithMaxWidth(s, 0.f, -0.875f, 1.7f, 0.7, cr, cg, cb);
			setMat4("transMat", glm::rotate(-1.57079633f, glm::vec3(0.f, 0.f, 1.f)));
			drawCenteredStringWithMaxWidth(s, 0.f, -0.875f, 1.7f, 0.7, cr, cg, cb);
			setMat4("transMat", glm::mat4(1.f));
		} else {
			res::eventOutline.bind();
			drawColoredTexture(-1.f, -0.6522547652254765, 2.f, 0.6522547652254765*2, embellishmentColor);
			
			res::eventSide.bind();
			drawColoredTexture(-1.f, -0.6522547652254765, 2.f, 0.6522547652254765*2, eventColor);
		}
	}
}
void drawTitle(char* ss) {
	s = string(ss);
	setFont("trajan");
	
	if(cardLayout == 0) {
		float r = 0, g = 0, b = 0;
		if(isTitleWhite) {
			r = 1;
			g = 1;
			b = 1;
		}
		
		drawCenteredStringWithMaxWidth(s, 0.f, 0.8f, 2.f, (strcmp(cardPreview, (char*)"") == 0) ? 1.f : 0.7f, r, g, b);
	} else if(cardLayout == 1) {
		if(!isTrait) {
			if(isTitleWhite) {
				drawCenteredStringWithMaxWidth(s, 0.f, 0.51f, 2.f, 0.66f, 1.f, 1.f, 1.f);
			} else {
				drawCenteredStringWithMaxWidth(s, 0.f, 0.51f, 2.f, 0.66f, 0.f, 0.f, 0.f);
			}
		}
	} else if(cardLayout == 2) {
		float r = 0, g = 0, b = 0;
		if(isBaseTitleWhite) {
			r = 1;
			g = 1;
			b = 1;
		}
		
		drawCenteredStringWithMaxWidth(s, 0.f, 0.8f, 2.f, (strcmp(cardPreview, (char*)"") == 0) ? 1.f : 0.7f, r, g, b);
	} else if(cardLayout == 3) {
		setMat4("transMat", glm::rotate(1.57079633f, glm::vec3(0.f, 0.f, 1.f)));
		drawCenteredStringWithMaxWidth(s, 0.f, 0.8f, 2.f, 0.5, 0.f, 0.f, 0.f);
		setMat4("transMat", glm::rotate(-1.57079633f, glm::vec3(0.f, 0.f, 1.f)));
		drawCenteredStringWithMaxWidth(s, 0.f, 0.8f, 2.f, 0.5, 0.f, 0.f, 0.f);
		setMat4("transMat", glm::mat4(1.f));
	} else if(cardLayout == 4) {
		isPermanentlyBold = true;
		drawCenteredStringWithMaxWidth(s, 0.f, 0.535f, 3.f, 1.f);
		isPermanentlyBold = false;
	}
}
void drawType(char* ss) {
	string sss = string(ss);
	string s1  = "";
	string s2  = "";
	bool isSplitting = getStringWidth(sss, 1.8f) > 0.8f;
	if(twoLinedType && cardLayout != 1 && isSplitting) {
		vector<string> lines = split(sss, " ");
		float halfWidth = getStringWidth(sss, 1.f)/2;
		float buildingWidth = 0.f;
		for(int i = 0; i < lines.size(); i++) {
			if(i < lines.size()/2+1) {
				s1 += lines[i] + " ";
			} else {
				s2 += lines[i] + (i == lines.size()-1 ? "" : " ");
			}
		}
	}
	setFont("trajan");
	isDrawingLargeIcons = false;
	
	float costSize = getStringWidth(string(cardCost), 2.5f);
	float typeX = 0.f;
	float typeWidth = 0.8f;
	if(costSize > 0.1f) {
		typeWidth -= (costSize-0.1f);
		typeX     += (costSize-0.1f)/2;
	}
	float cr = (isTitleWhite ? 1.f : 0.f);
	float cg = (isTitleWhite ? 1.f : 0.f);
	float cb = (isTitleWhite ? 1.f : 0.f);
	if(cardLayout == 0 || cardLayout == 2) {
		if(twoLinedType && isSplitting) {
			drawCenteredStringWithMaxWidth(s1, typeX, -0.753f, 1.3f, typeWidth, cr, cg, cb);
			drawCenteredStringWithMaxWidth(s2, typeX, -0.812f, 1.3f, typeWidth, cr, cg, cb);
		} else {
			drawCenteredStringWithMaxWidth(sss, typeX, -0.785f, 1.8, typeWidth, cr, cg, cb);
		}
	} else if(cardLayout == 1) {
		setMat4("transMat", glm::rotate(0.78539805f, glm::vec3(0.f, 0.f, 1.f)));
		drawCenteredStringWithMaxWidth(sss, 0.24f, 0.85f, 1.5f, 0.27, cr, cg, cb);
		setMat4("transMat", glm::mat4(1.f));
	}
	isDrawingLargeIcons = true;
}
void drawCardText() {
	string ct = string(cardText);
	
	setFont("tnr");
	float r = 0.f, g = 0.f, b = 0.f;
	if(isTextWhite) {
		r = 1.f;
		g = 1.f;
		b = 1.f;
	}
	
	if(cardLayout == 0) {
		ct += (isSupply ? "" : "\n[i](This is not in the Supply)");
		float size = 1.5f;
		drawCenteredStringWithMaxDimensions(
			(ct),
			textXPosTweak, -0.4f+textYPosTweak,         // X/Y
			size * textSizeTweak,// Scale
			1.8f * textSizeTweak, (0.6f - (strcmp("", heirloomText) == 0 ? 0.f : 0.06f)) * textSizeTweak,         // Max Dimensions
			r, g, b       // Color
		);
	} else if(cardLayout == 1) {
		float size = 1.5f;
		drawCenteredStringWithMaxDimensions(
			(ct),
			textXPosTweak, -0.415f+textYPosTweak,       // X/Y
			size * textSizeTweak,               // Scale
			2.5f, 0.25f,         // Max Dimensions
			r, g, b       // Color
		);
	} else if(cardLayout == 2) {
		float size = 8.f;
		largeIconSize = 8.f;
		drawCenteredStringWithMaxDimensions(
			string(ct),
			textXPosTweak, 0.3f+textYPosTweak,         // X/Y
			size * textSizeTweak,               // Scale
			1.8f, 0.6f,         // Max Dimensions
			r, g, b       // Color
		);
		largeIconSize = 4.f;
	} else if(cardLayout == 3) {
		// Leave empty (no text to be drawn)
	} else if(cardLayout == 4) {
		isDrawingLargeIcons = false;
		isPermanentlyBold = true;
		drawCenteredStringWithMaxDimensions(ct, -0.02f, -0.535f, 1.5f, 3.f * matWidthTweak, 0.25f);
		isDrawingLargeIcons = true;
		isPermanentlyBold = false;
	}
}
void drawCost() {
	isDrawingLargeIcons = false;
	if(cardLayout == 0 || cardLayout == 2) {
		drawString(string(cardCost), -0.53, -0.73, 2.5f);
	} else if(cardLayout == 1) {
		drawVerticallyCenteredText(string(cardCost), -0.85, 0.47, 2.5f);
	}
	isDrawingLargeIcons = true;
}
void drawPreview() {
	isDrawingLargeIcons = false;
	if(cardLayout == 0 || cardLayout == 2) {
		drawString(string(cardPreview), -0.52, 0.86, 2.5f);
		drawString(string(cardPreview), 0.52-getStringWidth(string(cardPreview), 2.5f), 0.86, 2.5f);
	} else if(cardLayout == 1) {
		// Leave empty
	}
	isDrawingLargeIcons = true;
}
void drawBottomTexts() {
	setFont("tnrb");
	if(cardLayout == 0 || cardLayout == 2) {
		drawVerticallyCenteredText(string(cardCredit), -0.5f, -0.89f, 0.6f * bottomTextSizeTweak, 1.f, 1.f, 1.f);
		drawRightAlignedVerticallyCenteredText(string(cardVersion), 0.5f, -0.89f, 0.6f * bottomTextSizeTweak, 1.f, 1.f, 1.f);
	} else if(cardLayout == 1) {
		drawVerticallyCenteredText(string(cardCredit), -0.825f, -0.525f, 0.6f * bottomTextSizeTweak);
		drawRightAlignedVerticallyCenteredText(string(cardVersion), 0.825f, -0.525f, 0.6f * bottomTextSizeTweak);
	}
	setFont("tnr");
}
void drawExpansionIcon() {
	if(cardLayout == 0) {
		if(res::tempExpansionIcon.id != 0) {
			res::tempExpansionIcon.bind();
			
			drawTexturedQuad(0.47f+expansionIconXPosTweak/25.f, -0.815f, 0.08f*expansionIconXSizeTweak, 0.08f*expansionIconYSizeTweak);
		}
	} else if(cardLayout == 1) {
		if(res::tempExpansionIcon.id != 0) {
			res::tempExpansionIcon.bind();
			
			drawTexturedQuad(0.763f+expansionIconXPosTweak/25.f, -0.495f, 0.07f*expansionIconXSizeTweak, 0.07f*expansionIconYSizeTweak);
		}
	} else if(cardLayout == 2) {
		if(res::tempExpansionIcon.id != 0) {
			res::tempExpansionIcon.bind();
			
			drawTexturedQuad(0.47f+expansionIconXPosTweak/25.f, -0.82f, 0.08f*expansionIconXSizeTweak, 0.08f*expansionIconYSizeTweak);
		}
	}
}
void MainDrawState::draw() {
	clear(0.f, 0.f, 0.f);
	
	glDisable(GL_DEPTH_TEST);
	
	doColors();
	
	drawBase();
	
	drawTitle(cardTitle);
	drawType(cardType);
	drawEmbellishments();
	drawCost();
	drawPreview();
	drawExpansionIcon();
	
	setFont("tnr");
	
	drawBottomTexts();
	res::vpToken.bind();
	drawCardText();
}
void MainDrawState::onKeyPress(int key, int action, int mods) {
    
}
void MainDrawState::onMouseClick(double x, double y, int action, int button) {

}
