#pragma once
#ifndef GL_IS_INCLUDED
#define GL_IS_INCLUDED
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

#include <string>

#include "Image.h"

using namespace std;

extern float width;
extern float height;

extern float pixelWidth;
extern float pixelHeight;

extern GLFWwindow* window;

extern float xStretch;
extern float yStretch;

extern float windowRatio, windowInverse;

extern bool testBool;
extern bool isFullscreen;

extern int currentFps;
extern int totalFramesEver;

extern Image* currentlyBindedImage;

float getHBorder();
float getYBorder();

void resetMatrix();

void screenShot();
void copyToClipboard();
void reloadPictures();

void openErrorPopup(string whoops);

extern float delta;
extern float drawDelta, updateDelta;
extern float averageDrawDelta, averageUpdateDelta;
extern float progressThroughFrame;
extern float targetFixedFps;

extern bool shouldRedraw;

extern string pathPrefix;

extern char* cardTitle;
extern char* cardType;
extern char* cardText;
extern char* iconUrl;
extern char* expansionUrl;
extern char* cardCost;
extern char* cardPreview;
extern char* cardVersion;
extern char* cardCredit;
extern char* heirloomText;
extern char* matText;
extern int   cardColor;
extern int   cardSecondary;
extern int   matColor;
extern float xMove;
extern float yMove;
extern float zoom;
extern int   cardLayout;
extern bool  isLowRes;
extern bool  isTraveler;
extern float textXTweak;
extern float textSizeTweak;
extern float bonusSizeTweak;
extern float textXPosTweak;
extern float textYPosTweak;
extern float expansionIconXSizeTweak;
extern float expansionIconYSizeTweak;
extern float expansionIconXPosTweak;
extern bool  largeSingleLineVanillaBonuses;
extern float tweakDividingLineY;
extern float bottomTextSizeTweak;
extern bool  isSupply;
extern bool  isTrait;
extern float matWidthTweak;
extern bool  twoLinedType;
extern float newlineSizeTweak;

extern bool  hasImage;
extern bool  isExample;

extern float customCardColor[];
extern float customEmbellishmentColor[];
extern float customSideColor[];
extern float secondCustomCardColor[];

extern string currentFile;

extern int  exampleSelected;
extern int  currentMenuTypee;
extern int  currentMenuType;
extern const char* examplesNames[];
extern char* recentFiles[5];
extern string recentFilesBeautified[5];
extern int numberOfRecentFiles;

extern bool isFirstFrame;
extern bool isDemoShown;