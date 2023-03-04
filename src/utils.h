#ifndef GL_IS_INCLUDED
#define GL_IS_INCLUDED
#include <GLFW/glfw3.h>
#endif
#include <string>
#include "Resources.h"

#include <glm/glm.hpp>

#include "Image.h"

using namespace std;

#ifndef UTILS_H_
#define UTILS_H_

struct Point2 {
    float x;
    float y;
};

struct GameSettings {
	bool shouldCrash;
	int hits;
	double money;
	float power;
	float gravity;
	float moneyGain;
	bool canGainHits;
	string savePath;
	string description;
};

struct ImageStruct {
	int width;
	int height;
	unsigned int textureId;
};

extern float allRed[];
extern float allGray[];
extern float allBlack[];
extern float allBlue[];
extern float allWhite[];
extern float allGreen[];
extern float allYellow[];
extern float allBrown[];
extern float allDarkGray[];
extern float eventColor[];

#endif

namespace bg {
	void drawAllPoints();
	void updateAllPoints();
}

extern int shaderProgram;
extern GameSettings currentSettings;
extern double moneyMadeThisGame;
extern bool isFixedFunction;

double getMouseX();
double getMouseY();
double getxPos(double xPos);
double getyPos(double yPos);
double getY();
double getX();
const char* readFile(char* file);
void checkShader(int shader);
void setMatrix(char* name, const float* data);
void setBool(string name, bool value);
void setInt(string name, int value);
void setFloat(string name, float value);
void setVec2(string name, float value1, float value2);
void setVec3(string name, float value1, float value2, float value3);
void setVec4(string name, float value1, float value2, float value3, float value4);
void setMat4(string name, glm::mat4 in);
int shaders(string vs, string fs);
int shaders();
ImageStruct loadImageToInt(string source, bool hasTrans);
Image loadImage(string source, bool hasTrans, /* IMAGE_TYPE */int type);
void clear(float r, float g, float b);
Point2 rotatePoint(Point2 in, float angle, float cx, float cy);
void plotPoint(Point2 in);
void fillOval(float x, float y, float z, float width, float height);
void drawTexturedQuad(float x, float y, float z, float w, float h);
void drawTexturedQuad(float x, float y, float w, float h);
void drawColoredTexture(float x, float y, float z, float wid, float hei, float rgb[12]);
void drawColoredTexture(float x, float y, float wid, float hei, float rgb[12]);
void drawColoredQuad(float x, float y, float z, float wid, float hei, float rgb[12]);
void drawColoredQuad(float x, float y, float wid, float hei, float rgb[12]);
void plotPoints(Point2 in[4]);
void plotPoints(Point2 in[4], float z);
void setTint(float r, float g, float b);
void drawPlayButton();
string doubleToString(double d, int precision);
string doubleToString(double d);
void bangExit();
void setupVertexPointers(int size);
int setupVAO();
void utilsSetup();
void translate(float x, float y);
bool startsWith(string og, string doesIt);
float getRandom();
void setAlpha(float one, float two, float three, float four);
void setAlpha(float al);
void setOob(bool oo);
char* readFileToChar(string path);
void drawNote(string note, float maxWidth, bool hasBackgroud, bool clearsDepthBit);
void drawSpotlights();
void drawRoundedRectangle(float x, float y, float z, float width, float height, float bearingX, float bearingY, float* color);
vector<string> split(string og, string splitter);
string split(string og, string splitter, bool isFirst);
void setTexPositions(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
void loadIcon(string url, string fileOut, Image* toLoad, bool isLoaded);