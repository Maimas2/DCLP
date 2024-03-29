#ifndef GL_IS_INCLUDED
#define GL_IS_INCLUDED
#include "curl/curl.h"
#include "glm/geometric.hpp"
#include <cstdint>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sys/wait.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <thread>
#include <signal.h>

#include "main.h"
#include "utils.h"
#include "handler.h"
#include "Text.h"
#include "Resources.h"
#include "Log.h"
#include "OSspecifics.h"
#include "Saves.h"
#include "ImguiManager.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "clip.h"

using namespace std;

GLFWwindow* createWindow(bool f);
static void changeFullscreen();
void calculateFPS();

bool isInfinite = false;

float baseWindowRatio = 0.6522547652254765;

float width;
float height;

float pixelWidth;
float pixelHeight;

float xStretch;
float yStretch;

GLFWwindow* window;

int currentSecond = 0;
int framesSoFar = 0;
int currentFps = 0;
int totalFramesEver = 0;

float windowRatio, windowInverse;

float delta = 0.f;
float drawDelta = 0.f, updateDelta = 0.f;
float averageDrawDelta = 0.f, averageUpdateDelta = 0.f;
float drawDeltaAdd = 0.f;
float updateDeltaAdd = 0.f;
float progressThroughFrame = 0.f;
float timeOfLastFrame = 0.f;
float targetFixedFps = 60.f;

bool isWindowFocused = true;
string pathPrefix;

bool isFirstFrame = true;

bool shouldRedraw = false;

float customCardColor[3] = {1.f, 1.f, 1.f};
float customEmbellishmentColor[3] = {1.f, 1.f, 1.f};
float customSideColor[3] = {0.897f, 0.621f, 0.413f,};

float secondCustomCardColor[3] = {1.f, 1.f, 1.f};

char* recentFiles[5];
string recentFilesBeautified[5];
int numberOfRecentFiles = 0;

Image* currentlyBindedImage = nullptr;

void resetColors() {
	for(int i = 0; i < 12; i++) {
		customCardColor[i] = 1.f;
		customEmbellishmentColor[i] = 1.f;
		secondCustomCardColor[i] = 1.f;
		eventColor[i] = beventColor[i];
	}
	customSideColor[0] = 0.897f;
	customSideColor[1] = 0.621f;
	customSideColor[2] = 0.413f;
}

// int getChoice(char* c) {
// 	for(int i = 0; i < 18; i++) {
// 		if(strcmp(mainChoices[i], c) == 0) return i;
// 	}
// 	return 0;
// }
static void enablings() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
    glEnable(GL_MULTISAMPLE);
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}
float lastFixedUpdate = 0.f;
static void update() {
	if(glfwGetTime()-lastFixedUpdate > 1.f/60.f) {
		handlerUpdate();
		lastFixedUpdate += 1.f/60.f;
	}
	progressThroughFrame = (glfwGetTime()-lastFixedUpdate)*targetFixedFps;
}
void checkFBO() {                                                         
	GLenum s = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER); 
	if(s == GL_FRAMEBUFFER_COMPLETE) {
		return;
	} else if(s == GL_FRAMEBUFFER_UNSUPPORTED) {
		return;
	} else {
		Log::fatal("FBO error!", true);
	}
}
struct FBO {
	GLuint fbo;
	GLuint color;
	GLuint depth;
	int width;
	int height;
};
FBO* setupFBO(int fboWidth, int fboHeight) {
	FBO* toReturn = new FBO;
	toReturn->width = fboWidth;
	toReturn->height = fboHeight;
	glGenFramebuffers(1, &(toReturn->fbo));
	glGenTextures(1, &(toReturn->color));
	glGenRenderbuffers(1, &(toReturn->depth));

	glBindFramebuffer(GL_FRAMEBUFFER, (toReturn->fbo));

	glBindTexture(GL_TEXTURE_2D, (toReturn->color));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboWidth, fboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (toReturn->color), 0);

	glBindRenderbuffer(GL_RENDERBUFFER, (toReturn->depth));
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, fboWidth, fboHeight);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, (toReturn->depth));

	return toReturn;
}
int w = 0, h = 0;
bool screenshottingNextFrame = false;
int iwidth, iheight;
GLubyte* getNonAlphaScreenshotPixels() {
	GLubyte* pixels = (GLubyte*)malloc(3 * w * h);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	if (GL_NO_ERROR != glGetError()) throw "Error: Unable to read pixels.";

	return pixels;
}
void doWidthHeightPixels() {
	if(cardLayout == 0 || cardLayout == 2) {
		w = 1403;
		h = 2151;
		baseWindowRatio = 1403.f/2151.f;
	} else if(cardLayout == 1 || cardLayout == 3) {
		w = 2151;
		h = 1403;
		baseWindowRatio = 2151.f/1403.f;
	}else if(cardLayout == 4) {
		w = 2151;
		h = 1564;
		baseWindowRatio = 1.375385344;
	}
}
GLubyte* getScreenshotPixels() {
	GLubyte* pixels = (GLubyte*)malloc(3 * w * h);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	if (GL_NO_ERROR != glGetError()) throw "Error: Unable to read pixels.";

	return pixels;
}
void draw();
FBO* uprightCardFBO = nullptr, *uprightDrawingFbo = nullptr;
FBO* landscapeCardFBO = nullptr, *landscapeDrawingFBO = nullptr;
FBO* matCardFBO = nullptr, *matDrawingFBO = nullptr;
void drawOnFbo(bool isFlipping) {
	if(uprightCardFBO == nullptr) {
		uprightCardFBO      = setupFBO(1403, 2151);
		uprightDrawingFbo   = setupFBO(1403, 2151);

		landscapeCardFBO    = setupFBO(2151, 1403);
		landscapeDrawingFBO = setupFBO(2151, 1403);

		matCardFBO          = setupFBO(2151, 1564);
		matDrawingFBO       = setupFBO(2151, 1564);
	}
	FBO* currentFbo = nullptr;
	FBO* drawingFbo = nullptr;
	if(cardLayout == 0 || cardLayout == 2){
		currentFbo = uprightCardFBO;
		drawingFbo = uprightDrawingFbo;
	}
	if(cardLayout == 1 || cardLayout == 3) {
		currentFbo = landscapeCardFBO;
		drawingFbo = landscapeDrawingFBO;
	}
	if(cardLayout == 4) {
		currentFbo = matCardFBO;
		drawingFbo = matDrawingFBO;
	}
	assert(currentFbo != nullptr);
	assert(drawingFbo != nullptr);
	glBindFramebuffer(GL_FRAMEBUFFER, currentFbo->fbo);
	glViewport(0, 0, currentFbo->width, currentFbo->height);

	doWidthHeightPixels();

	iwidth = currentFbo->width;
	iheight = currentFbo->height;
	pixelWidth = (float)iwidth;
	pixelHeight = (float)iheight;

	resetMatrix();

	if(cardLayout == 1 || cardLayout == 3) {
		xStretch = 1.f;
		yStretch = 1403.f/2151.f;
		setFloat("xStretch", xStretch);
		setFloat("yStretch", yStretch);
	} else if(cardLayout == 4) {
		xStretch = 1.f;
		yStretch = 0.727068966f;
		setFloat("xStretch", xStretch);
		setFloat("yStretch", yStretch);
	}

	enablings();

	clear(0.f, 0.f, 0.f);

	handlerDraw();

	glBindFramebuffer(GL_FRAMEBUFFER, drawingFbo->fbo);

	glViewport(0, 0, drawingFbo->width, drawingFbo->height);

	glm::mat4 tp = glm::mat4(1.f);
	if(isFlipping) tp *= glm::scale(glm::vec3(-1.f, 1.f, 1.f));
	setMat4("transMat", tp);

	if(cardLayout == 4) {
		xStretch = 1.f;
		yStretch = 0.727068966f;
	}

	clear(0.f, 0.f, 0.f);

	glBindTexture(GL_TEXTURE_2D, currentFbo->color);
	drawTexturedQuad(-xStretch, yStretch, xStretch*2, -yStretch*2);
	if(isFlipping) setMat4("transMat", glm::mat4(1.f));
}
void screenShot() {
	doWidthHeightPixels();

	drawOnFbo(false);
	GLubyte* pixels = getScreenshotPixels();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	stbi_flip_vertically_on_write(true);
	stbi_write_jpg("out.jpg", w, h, 3, pixels, 100);
	
	free(pixels);
}
void copyToClipboard() {
	doWidthHeightPixels();

	setMat4("baseTransMat", glm::scale(glm::vec3(-1.f, 1.f, 1.f)));
	drawOnFbo(true);
	GLubyte* pixels = getNonAlphaScreenshotPixels();

	uint8_t* pix = (uint8_t*)malloc(w * h * sizeof(uint8_t) * 4);

	int m = w * h * 4;
	int g = 0;
	for(int i = m-1; i > 0; i--) {
		if(i % 4 == 3) {
			pix[i] = 255;
			continue;
		}
		int pn = (g++) / 3;
		int r  = (pn / h);
		int c  = (pn % h);
		pix[i] = pixels[((r * h) + c)*3 + (i % 4)];
	}

	clip::image_spec spec;
	spec.width = w;
	spec.height = h;
	spec.bits_per_pixel = 32;
	spec.bytes_per_row = spec.width*4;
	spec.red_mask = 0xff;
	spec.green_mask = 0xff00;
	spec.blue_mask = 0xff0000;
	spec.alpha_mask = 0xff000000;
	spec.red_shift = 0;
	spec.green_shift = 8;
	spec.blue_shift = 16;
	spec.alpha_shift = 24;
	clip::image img(pix, spec);
	clip::set_image(img);

	free(pixels);
	free(pix);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	setMat4("baseTransMat", glm::mat4(1.f));
}
char* cardTitle   = (char*)malloc(128);  // This is used around the program as a temporary storage for strings.
char* cardType    = (char*)malloc(128);
char* cardText    = (char*)malloc(512);
char* matText     = (char*)malloc(512);
char* iconUrl     = (char*)malloc(512);
char* expansionUrl= (char*)malloc(512);
char* cardCost    = (char*)malloc(32);
char* cardPreview = (char*)malloc(32);
char* cardCredit  = (char*)malloc(128);
char* cardVersion = (char*)malloc(128);
char* heirloomText= (char*)malloc(128);
int   cardColor = 0;
int   cardSecondary = 0;
int   cardLayout= 0;
int   matColor  = 0;
float xMove     = 0.f;
float yMove     = 0.f;
float zoom      = 1.f;
bool  isLowRes  = true;
bool  showWindow= true;
bool  isTraveler= false;
bool  isTightScreenshot = false;
float textXTweak= 1.f;
float textSizeTweak = 1.f;
float bonusSizeTweak= 1.f;
float textXPosTweak = 0.f;
float textYPosTweak = 0.f;
float expansionIconXSizeTweak = 1.f;
float expansionIconYSizeTweak = 1.f;
float expansionIconXPosTweak  = 0.f;
bool  largeSingleLineVanillaBonuses = true;
float tweakDividingLineY = -0.4f;
float bottomTextSizeTweak = 1.f;
bool  isSupply  = true;
bool  isTrait   = false;
bool  hasImage  = false;
float matWidthTweak = 1.f;
bool  twoLinedType = false;
float newlineSizeTweak = 1.f;

float lastResetClick;
int   exampleSelected = 0;
bool  isExample = false;

bool isOtherWindowShwon = false;
bool isDemoShown = false;

string currentFile = "";

void reloadPictures() {
	loadIcon(string(iconUrl), "./tempicon.png", &(res::tempIcon), false);
	loadIcon(string(expansionUrl), "./expansionicon.png", &(res::tempExpansionIcon), false);
	res::tempExpansionIcon = res::setupImage("./expansionicon.png", true);
	if(strcmp(iconUrl, "") != 0) {
		hasImage = true;
	} else hasImage = false;
}
int currentMenuTypee = 0;
int currentMenuType  = 0;

void composeDearImGuiFrame() {
    doImguiWindow();
}
string whoopsMessage;
void openErrorPopup(string whoops) {
	ImGui::OpenPopup("Whoops");
	whoopsMessage = whoops;
}
void draw() {
    if(uiMode > 0) {
		handlerDraw();
	} else {
		clear(0.f, 0.f, 0.f);
	}
	
	composeDearImGuiFrame();

	if(ImGui::BeginPopupModal("Whoops")) {
		ImGui::Text("%s", whoopsMessage.c_str());
		if(ImGui::Button("My bad")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	
    ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}
void changeFullscreen() {
	if(!isFullscreen) {
		const GLFWvidmode* v = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, v->width, v->height, 0);
	} else {
		const GLFWvidmode* v = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowMonitor(window, nullptr, 0, 0, 1600, 900, 0);
		glfwMaximizeWindow(window);
	}
	isFullscreen = !isFullscreen;
	if(isFullscreen) {
		Log::debug("Entering fullscreen.");
	} else {
		Log::debug("Exiting fullscreen.");
	}
}
bool testBool = false;
bool isFullscreen = false;
bool hasDebugMode = false;
void key_callback(GLFWwindow* eventWindow, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_S && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
	   	Log::log("Ctrl+Alt+S pressed, segfaulting. This will kill everything in its place, probably causing things like data loss and stuff.");
	   	Log::flushFile();

		cout << "Ctrl+Alt+S pressed, initiating segfault." << endl;

       	raise(11);
    }
    if(key == GLFW_KEY_F4 && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        dclpExit();
        return;
    }
	shouldRedraw = true;
    if(action != GLFW_PRESS) {
        return;
    }
	if(key == GLFW_KEY_F11) {
		changeFullscreen();
	}
    if(key == GLFW_KEY_SPACE) {
        testBool = !testBool;
		Log::debug("Setting testBool to " + to_string(testBool));
    }
    onKey(key, action, mods);
}
void window_size_callback(GLFWwindow* window, int width, int height) {
    Log::debug("Window resized to " + doubleToString(width) + "x" + doubleToString(height));
	setFloat("pixelWidth", pixelWidth);
	setFloat("pixelHeight", pixelHeight);
	shouldRedraw = true;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(action == GLFW_PRESS) {
		onMouse(getX(), getY(), action, button);
	}
	shouldRedraw = true;
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	shouldRedraw = true;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	shouldRedraw = true;
}
void window_focus_callback(GLFWwindow* window, int focused) {
    if(focused) {
        isWindowFocused = true;
		Log::debug("Window focused");
    } else {
        isWindowFocused = false;
		Log::debug("Window unfocused");
    }
	shouldRedraw = true;
}
void calculateFPS() {
    totalFramesEver++;
    if(floor(glfwGetTime()) > currentSecond) {
		averageUpdateDelta = updateDeltaAdd / (float)framesSoFar;
		averageDrawDelta = drawDeltaAdd / (float)framesSoFar;
		
        currentSecond = floor(glfwGetTime());
        currentFps = framesSoFar;
        framesSoFar = 0;
		
		drawDeltaAdd = 0.f;
		updateDeltaAdd = 0.f;
		
		Log::flushFile();
    } else {
        framesSoFar++;
    }
}
float getHBorder() {
    if(windowRatio > baseWindowRatio) {
        return windowRatio;
    } else {
        return baseWindowRatio;
    }
}
float getYBorder() {
    if(windowRatio > baseWindowRatio) {
        return 1.f;
    } else {
        return baseWindowRatio * windowInverse;
    }
}
GLFWwindow* createWindow(bool f) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_REFRESH_RATE, 60);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	const GLFWvidmode* v = glfwGetVideoMode(glfwGetPrimaryMonitor());
	Log::debug("Refresh rate should be " + to_string(v->refreshRate) + "hz");
	GLFWwindow* e;
	
	e = glfwCreateWindow(v->width, v->height, "DCLP", NULL, NULL);
	if (!e)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(e);
    glfwSetKeyCallback(e, key_callback);
    glfwSetMouseButtonCallback(e, mouse_button_callback);
    glfwSetWindowSizeCallback(e, window_size_callback);
	glfwSetScrollCallback(e, scroll_callback);
	glfwSetWindowFocusCallback(e, window_focus_callback);
	glfwSetCursorPosCallback(e, cursor_position_callback);

	return e;
}
void getWindowSize() {
	glfwGetFramebufferSize(window, &iwidth, &iheight);
}
void resetMatrix() {
	windowRatio = (float) iwidth / (float) iheight;
	windowInverse = (float)iheight / (float)iwidth;
	glViewport(0, 0, iwidth, iheight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	width = getHBorder()*2.f;
	height = getYBorder()*2.f;

	pixelWidth = (float)iwidth;
	pixelHeight = (float)iheight;

	if(windowRatio > baseWindowRatio) {
		xStretch = getHBorder();
		yStretch = 1.f;
	} else {
		xStretch = baseWindowRatio;
		yStretch = getYBorder();
	}
	setFloat("xStretch", xStretch);
	setFloat("yStretch", yStretch);
	
	glUseProgram(shaderProgram);
}
void imguiInit() {
	//Uhh turns out this is taken from the provided ImGui examples, under the MIT license
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}
#define incrementLoop() i++;if(i>=argc) break;
void printHelp() {
	cout << "Dominion Command Line Card Creator" << endl <<
			"This program is used to generate cards for Dominion, by Donald X. Vaccarino." << endl <<
			"------- COPYRIGHT NOTICE -------" << endl <<
			"All designs of cards are property of Donald X. Vaccarino and Rio Grande Games." << endl <<
			"-- Other Notes --" << endl <<
			"All designs of cards are takes from shardofhonor's similar card creator, found at https://shardofhonor.github.io/dominion-card-generator/" << endl <<
			"This program uses Curl for downloading images. Use --curl-license for curl's license." << endl <<
			"Unless specified with --high-res, all images (excluding the coin, debt, and VP token) are rendered at half resolution." << endl << 
			"If any argument specifies a description of the card, it will automatically take a screenshot. To enter the editor, use no arguments specifying the description." << endl << endl <<
			"If a money amount is empty, try putting a backslash before the dollar signs so they are not intreted as shell variables." << endl <<
			"---- ARGUMENTS ----" << endl <<
			//"--cli\t\t\tEnter alternate CLI" << endl <<
			"--title\t\t\tSelect title of card" << endl <<
			"--type\t\t\tSelect type of card. Use --list-types to see avalible types." << endl <<
			"--cost\t\t\tSelect cost of card. Use --legend for symbols." << endl <<
			"--preview\t\tSelect preview of card (This is the symbol in the top left and right corners of the card). Use --legend for symbols." << endl <<
			"--preview\t\tSelect preview of card (This is the symbol in the top left and right corners of the card). Use --legend for symbols." << endl <<
			"--url\t\t\tSelect url of picture of card. Note: Uses curl to download images." << endl <<
			"--expansion-url\tSelect url of expansion of card. (The bottom right icon) Note: Uses curl to download images." << endl <<
			"--author\t\tSelect author and version card." << endl <<
			"--illustration-credit\tSelect illustration credit for card." << endl <<
			"--primary-color\t\tSelect primary color for card. Use --list-colors to see avalible colors." << endl <<
			"--secondary-color\tSelect secondary color for card. If unspecified, the primary color will be used. Use --list-colors to see avalible colors." << endl <<
			"--text\t\t\tSelect card text. See --legend for symbols." << endl << 
			"--layout\t\t\tSelect card layout. See --layouts for a list of layouts." << endl << 
			"--font-size\t\tSelect font size. Default is 144, using over 500 will result in warning." << endl << 
			"";
}
void printCurlLicense() {
	cout << "Curl (found at https://curl.se) license. This license can be found at https://curl.se/docs/copyright.html" << endl << "\
\
COPYRIGHT AND PERMISSION NOTICE\n\
\n\
Copyright (c) 1996 - 2022, Daniel Stenberg, daniel@haxx.se, and many contributors, see the THANKS file.\n\
\n\
All rights reserved.\n\
\n\
Permission to use, copy, modify, and distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies.\n\
\n\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\
\n\
Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization of the copyright holder." << endl;
}
// void printColors() {
// 	for(int i = 0; i < 18; i++) {
// 		cout << i+1 << "." << (i < 9 ? "  " : " ") << mainChoices[i] << endl;
// 	}
// }
// void printLegend() {
// 	cout << "--- LEGEND ---" << endl <<
// 			"$\tMoney symbol (Ya know, the coin)" << endl <<
// 			"@\tDebt symbol" << endl <<
// 			"^\tPotion symbol" << endl <<
// 			"%\tVictory point symbol" << endl;
// }
void printLayouts() {
	cout << "--- Dominion Card Layouts ---" << endl <<
			"0\tNormal type of card. Used by Actions, Reactions, etc." << endl <<
			"1\tLandscape/horizontal card. Used by Events, Projects, etc." << endl <<
			"2\tBase card. Used by Copper, Silver, Estate, Province, Curse, etc." << endl <<
			"";
}
string replaceAll(string in, string old, string news) {
	int place = 0;
	while(true) {
		if(in.find(old, place) == string::npos) break;
		in.replace((place = in.find(old, place)), news.size(), news);
	}
	return in;
}
int main(int argc, char *argv[]) {
	deleteFile("expansionicon.png");
	deleteFile("tempicon.png");
	showWindow = false;
	bool isCli = false;
	
	for(int i = 0; i < 512; i++) {
		if(i == 1) {
			char* cardText    = (char*)malloc(512);
			char* iconUrl     = (char*)malloc(512);
			char* expansionUrl= (char*)malloc(512);
		}
		if(i < 32) {
			cardCost[i] = '\0';
			cardPreview[i] = '\0';
		}
		if(i < 128) {
			cardTitle[i] = '\0';
			cardType[i] = '\0';
			cardCredit[i] = '\0';
			cardVersion[i] = '\0';
		}
		cardText[i] = '\0';
		iconUrl[i] = '\0';
		expansionUrl[i] = '\0';
	}

	for(int i = 0; i < 5; i++) {
		recentFiles[i] = (char*)malloc(512);
	}
	
	vector<string> splitted = split(getPathToExe(), "/");
	string pathPrefix;
	for(int i = 0; i < splitted.size()-1; i++) {
		pathPrefix += "/" + splitted[i];
	}
	Log::log("Path prefix should be " + pathPrefix);
	
	int logReturn = Log::loggerInit();
	if(logReturn != 0) {
		cout << "Logger init failed!" << endl;
	}

	curl_global_init(CURL_GLOBAL_ALL);
	CURL* handle = curl_easy_init();
	
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    //const GLFWvidmode* v = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window = createWindow(false);

	GLFWimage images[1]; 
	images[0].pixels = stbi_load("images/icon_64px.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(images[0].pixels);
    

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize OpenGL context! Exiting" << std::endl;
        return -1;
    }
	
   	textInit();
	loadFont("cinzel.ttf", "trajan");
	loadFont("cinzel-bold.ttf", "trajanb");
	loadFont("freefonts/FreeSerifBold.ttf", "tnrb");
	loadFont("freefonts/FreeSerif.ttf", "tnr");
	loadFont("freefonts/FreeSerifItalic.ttf", "tnri");
	loadFont("freefonts/FreeSerifBoldItalic.ttf", "tnrib");
	
	imguiInit();

	glClearColor(0.f, 0.f, 0.f, 0.0f);

	Saves::readFirst();
	
	utilsSetup();
	
	textInit();
	
	handOnLoad();

	reloadPictures();
	
	glfwSwapInterval(1);
	
	float deltaFloat = 0.f;

	setMat4("baseTransMat", glm::mat4(1.f));
	setMat4("transMat", glm::mat4(1.f));

	res::initial();

	if(argc > 1 && filesystem::exists(string(argv[1]))) {
		Saves::read(string(argv[1]));
		uiMode = 1;
		reloadPictures();
		for(int i = 0; i < 2; i++) {
			calculateFPS();
			enablings();
			
			getWindowSize();
			resetMatrix();

			update();
			draw();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		screenShot();
		dclpExit();
		return 0;
	}

	//res::initialLoad();

	glfwShowWindow(window);
	
    while (!glfwWindowShouldClose(window)) {
		calculateFPS();
        enablings();
		
		getWindowSize();
		resetMatrix();

		if(totalFramesEver == 0) {
			setFont("trajan");
			clear(0.f, 0.f, 0.f);
			drawCenteredString("Loading...", 0.f, 0.f, 4.f, 1.f, 0.f, 0.f);
			glfwSwapBuffers(window);
			totalFramesEver++;
			continue;
		}
		if(totalFramesEver == 1) {
			
		}
		
		deltaFloat = glfwGetTime();
		update();
		updateDelta = (glfwGetTime()-deltaFloat)*1000;
		updateDeltaAdd += updateDelta;
		
		deltaFloat = glfwGetTime();
		if(isWindowFocused) {// && shouldRedraw) {
			draw();
			drawDelta = (glfwGetTime() - deltaFloat)*1000;
			drawDeltaAdd += drawDelta;
			shouldRedraw = false;
		} else {
			drawDelta = 0.f;
		}
		
		delta = abs(glfwGetTime()-timeOfLastFrame);
        timeOfLastFrame = glfwGetTime();
		
		int timeToSleep = ((1.f/targetFixedFps)-delta)*1000;
		this_thread::sleep_for((chrono::milliseconds)timeToSleep);
		
        glfwSwapBuffers(window);
        glfwPollEvents();

		if(screenshottingNextFrame) {
			copyToClipboard();
		}
    }
	
	dclpExit();
    return 0;
}