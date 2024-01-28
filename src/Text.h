#pragma once
#include <string>

using namespace std;

extern float  currentFontHeight, fontDownscale;
extern string currentFont;
extern bool   isDrawingLargeIcons;
extern float  largeIconSize;
extern int    fontSize;
extern bool   isBold;
extern bool   isPermanentlyBold;

int textInit();
int loadFont(string fontPath, string name);

string strip(string in);

float getStringWidth(string in, float scale);
float getStringWidthRaw(string in, float scale);

void drawString(string text, float x, float y, float scale, float r, float g, float b);
void drawString(string text, float x, float y, float scale);

void drawCenteredStringSingleLine(string textt, float x, float y, float scale, float r, float g, float b);

void drawCenteredString(string text, float x, float y, float scale);
void drawCenteredString(string text, float x, float y, float scale, float r, float g, float b);

float getStringYMax(string in, float scale);
float getStringHeight(string in, float scale);
float getStringHeightRequired(string in, float scale);

bool shouldBeBolded(string in);
bool isLargeSymbol(string in);
float drawLargeIcon(string text, float x, float y);

void drawVerticallyCenteredText(string in, float x, float y, float scale, float r, float g, float b);
void drawVerticallyCenteredText(string text, float x, float y, float scale);

void drawRightAlignedVerticallyCenteredText(string in, float x, float y, float scale, float r, float g, float b);
void drawRightAlignedVerticallyCenteredText(string text, float x, float y, float scale);

void drawRightAlignedText(string in, float x, float y, float scale, float r, float g, float b);
void drawRightAlignedText(string in, float x, float y, float scale);

void drawLeftAlignedVerticallyCenteredText(string in, float x, float y, float scale, float r, float g, float b);
void drawLeftAlignedVerticallyCenteredText(string in, float x, float y, float scale);

string clampStringToWidth(string in, float width, float scale);

void drawCenteredStringWithMaxDimensions(string in, float x, float y, float scale, float maxWidth, float maxHeight, float r, float g, float b);
void drawCenteredStringWithMaxDimensions(string in, float x, float y, float scale, float maxWidth, float maxHeight);

void drawCenteredStringWithMaxWidth(string in, float x, float y, float scale, float maxWidth, float r, float g, float b);
void drawCenteredStringWithMaxWidth(string in, float x, float y, float scale, float maxWidth);

void yes();
void setFont(string nf);