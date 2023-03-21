#ifndef GL_IS_INCLUDED
#define GL_IS_INCLUDED
#include "glm/geometric.hpp"
#include <cstdint>
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
#include "artworks.h"

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

bool isPausedMaster = false;

bool isFirstFrame = true;

bool shouldRedraw = false;

#define NUMBER_OF_CHOICES 21
const char* mainChoices[] = {"Action/Event", "Treasure", "Victory", "Reaction", "Duration", "Reserve", "Curse", "Shelter", "Ruins", "Landmark", "Night", "Boon", "Hex", "State", "Artifact", "Project", "Way", "Ally", "Trait", "Custom", "Extra Custom"};
const char* secondaryChoices[] = {"Same", "Action/Event", "Treasure", "Victory", "Reaction", "Duration", "Reserve", "Curse", "Shelter", "Ruins", "Landmark", "Night", "Boon", "Hex", "State", "Artifact", "Project", "Way", "Ally", "Trait", "Custom"};
const char* layoutChoices[] = {"Normal", "Landscape", "Base Card", "Pile Marker", "Player Mat"};
const char* matColorChoices[] = {"Black", "Red", "Green", "Brown", "Blue"};

const char* examplesNames[] = {
	"Workshop",
	"Coin of the Realm",
	"Bonfire",
	"Dominate",
	"Cursed (trait)",
	"Curse (base card)",
	"Bane Marker",
};
const char* examplesUrls[] = {
	"examples/workshop.dclp",
	"examples/coin.dclp",
	"examples/bonfire.dclp",
	"examples/dominate.dclp",
	"examples/cursed.dclp",
	"examples/curse.dclp",
	"examples/bane.dclp",
};

float customCardColor[3] = {1.f, 1.f, 1.f};
float customEmbellishmentColor[3] = {1.f, 1.f, 1.f};
float customSideColor[3] = {1.2f, 0.8f, 0.5f};

float secondCustomCardColor[3] = {1.f, 1.f, 1.f};

int getChoice(char* c) {
	for(int i = 0; i < 18; i++) {
		if(strcmp(mainChoices[i], c) == 0) return i;
	}
	return 0;
}
static void enablings() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    handlerUnfixedUpdate();
}
int w, h;
bool screenshottingNextFrame = false;
GLubyte* getScreenshotPixels() { // Based off https://stackoverflow.com/questions/33757634/creating-screenshot-in-opengl-not-working
	if(!isFullscreen && !isScreenshotting) {
		changeFullscreen();
		calculateFPS();
        enablings();
		for(int i = 0; i < 10; i++) {
			resetMatrix();
			update();
			handlerDraw();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	if(cardLayout == 0 || cardLayout == 2) {
		w = (pixelWidth/1920)*672;
		h = (pixelHeight/1080)*1080;
	} else if(cardLayout == 1 || cardLayout == 3) {
		w = (pixelWidth/1920)*1080;
		h = (pixelHeight/1080)*702;
	}
	GLubyte* pixels = (GLubyte*)malloc(3 * w * h);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glReadPixels((int)(pixelWidth/2)-(w/2), (int)(pixelHeight/2)-(h/2), w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	if (GL_NO_ERROR != glGetError()) throw "Error: Unable to read pixels.";

	return pixels;
}
void screenShot() {
	GLubyte* pixels = getScreenshotPixels();

	stbi_flip_vertically_on_write(true);
	stbi_write_jpg("out.jpg", w, h, 3, pixels, 100);
	
	free(pixels);
}
void copyToClipboard() {
	if(!screenshottingNextFrame) {
		screenshottingNextFrame = true;
		return;
	}
	GLubyte* pixels = getScreenshotPixels();

	uint8_t* pix = (uint8_t*)malloc(w * h * sizeof(uint8_t) * 4);

	int m = w * h * 4;
	int g = 0;
	for(int i = 0; i < m; i++) {
		if(i % 4 == 3) {
			pix[i] = 255;
			continue;
		}
		int pn = ((w * h * 3) - g++)/ 3;
		int r  = (pn / h);
		int c  = (pn % h);
		//cout << g-1 << "\t" << ((r * h) + c)*3 << endl;
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

	screenshottingNextFrame = false;
}
char* cardTitle   = (char*)malloc(128);
char* cardType    = (char*)malloc(128);
char* cardText    = (char*)malloc(512);
char* iconUrl     = (char*)malloc(512);
char* expansionUrl= (char*)malloc(512);
char* cardCost    = (char*)malloc(32);
char* cardPreview = (char*)malloc(32);
char* cardCredit  = (char*)malloc(128);
char* cardVersion = (char*)malloc(128);
char* artSearch   = (char*)malloc(128);
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
bool  largeSingleLineVanillaBonuses = true;
float tweakDividingLineY = -0.4f;
float bottomTextSizeTweak = 1.f;
bool  isSupply  = true;
bool  isTrait   = false;

float lastResetClick;
int imageToLoad = 0;
int exampleSelected = 0;

char** shownArtworks = (char**)malloc(sizeof(char*) * 700);
int p = 0;
bool isOtherWindowShwon = false;

char* expansionIconList[] = {
	(char*)"Adventures",
	(char*)"Alchemy",
	(char*)"Allies",
	(char*)"Base cards",
	(char*)"Base set",
	(char*)"Base set (old)",
	(char*)"Cornucopia",
	(char*)"Dark ages",
	(char*)"Empires",
	(char*)"Guilds",
	(char*)"Hinterlands",
	(char*)"Hinterlands (old)",
	(char*)"Intrigue",
	(char*)"Intrigue (old)",
	(char*)"Menagerie",
	(char*)"Nocturne",
	(char*)"Plunder",
	(char*)"Promo",
	(char*)"Prosperity",
	(char*)"Prosperity (old)",
	(char*)"Renaissance",
	(char*)"Seaside",
	(char*)"Seaside (old)."
};

char* expansionIconListImage[] = {
	(char*)"expansion-images/adventures.png",
	(char*)"expansion-images/alchemy.png",
	(char*)"expansion-images/allies.png",
	(char*)"expansion-images/base-cards.png",
	(char*)"expansion-images/base-set.png",
	(char*)"expansion-images/base-set-old.png",
	(char*)"expansion-images/cornucopia.png",
	(char*)"expansion-images/dark-ages.png",
	(char*)"expansion-images/empires.png",
	(char*)"expansion-images/guilds.png",
	(char*)"expansion-images/hinterlands.png",
	(char*)"expansion-images/hinterlands-old.png",
	(char*)"expansion-images/intrigue.png",
	(char*)"expansion-images/intrigue-old.png",
	(char*)"expansion-images/menagerie.png",
	(char*)"expansion-images/nocturne.png",
	(char*)"expansion-images/plunder.png",
	(char*)"expansion-images/promo.png",
	(char*)"expansion-images/prosperity.png",
	(char*)"expansion-images/prosperity-old.png",
	(char*)"expansion-images/renaissance.png",
	(char*)"expansion-images/seaside.png",
	(char*)"expansion-images/seaside-old.png"
};

char *trimwhitespace(char *str) { // https://stackoverflow.com/a/122721
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}
void reloadPictures() {
	loadIcon(string(iconUrl), "./tempicon.png", &(res::tempIcon), false);
	loadIcon(string(expansionUrl), "./expansionicon.png", &(res::tempExpansionIcon), false);
}
void composeDearImGuiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    {
		ImGui::SetNextWindowBgAlpha(0.7f);
        
        ImGui::Begin("Options", NULL);

		if(ImGui::Button("Load Example")) {
			ImGui::OpenPopup("Load Example");
		}
		
		ImGui::ListBox("Layout", &cardLayout, layoutChoices, IM_ARRAYSIZE(layoutChoices), 5);
		
		if(cardLayout == 0) ImGui::Checkbox("Is Supply Card?", &isSupply);
		if(cardLayout == 0) ImGui::Checkbox("Traveller?", &isTraveler);
		if(cardLayout == 1) ImGui::Checkbox("Trait?", &isTrait);
		
		ImGui::InputText("Title", cardTitle, 100);
		
		if(cardLayout <= 2) ImGui::InputText("Type", cardType, 100);
		if(cardLayout == 2 || cardLayout == 0 || (cardLayout == 1 && !isTrait)) ImGui::InputText("Cost", cardCost, 30);
		
		if(cardLayout != 3) ImGui::InputText("Art Credit", cardCredit, 120);
		if(cardLayout != 3) ImGui::InputText("Card Version and Creator", cardVersion, 120);
		if(cardLayout != 3) ImGui::InputText("Heirloom", heirloomText, 120);
		
		if(cardLayout == 0 || cardLayout == 2) ImGui::InputText("Preview (Top left & right)", cardPreview, 30);
		
		if(cardLayout <= 3) ImGui::ListBox("Color", &cardColor, mainChoices, IM_ARRAYSIZE(mainChoices), 6);
		if(cardLayout < 2) {
			if(cardLayout == 1) {
				if(!isTrait) ImGui::ListBox("Secondary Color", &cardSecondary, secondaryChoices, IM_ARRAYSIZE(secondaryChoices), 6);
			} else {
				ImGui::ListBox("Secondary Color", &cardSecondary, secondaryChoices, IM_ARRAYSIZE(secondaryChoices), 6);
			}
		}
		if(cardLayout == 4) ImGui::ListBox("Mat Color", &matColor, matColorChoices, IM_ARRAYSIZE(matColorChoices), 6);

		if(cardColor >= NUMBER_OF_CHOICES-2 && cardLayout < 4) {
			ImGui::ColorEdit3("Card Base Color", customCardColor);
			if(cardColor == NUMBER_OF_CHOICES-1) {
				ImGui::ColorEdit3("Card Embellishment Color", customEmbellishmentColor);
				if(cardLayout != 3) ImGui::ColorEdit3("Card Side Color", customSideColor);
			}
		}
		if(cardSecondary+1 == NUMBER_OF_CHOICES && cardLayout < 2 && !isTrait) {
			ImGui::ColorEdit3("Secondary Card Base Color", secondCustomCardColor);
		}
		if(cardColor >= NUMBER_OF_CHOICES-2 && cardLayout < 4) {
			if(ImGui::Button("Reset Colors")) {
				for(int i = 0; i < 12; i++) {
					customCardColor[i] = 1.f;
					customEmbellishmentColor[i] = 1.f;
					customSideColor[i] = allBrown[i];
					secondCustomCardColor[i] = 1.f;
				}
			}
		}
		
		if(cardLayout <= 2) ImGui::InputTextMultiline("Card Text", cardText, 500, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_None);
		
		ImGui::InputText("Picture URL", iconUrl, 500);
		if(cardLayout <= 2) ImGui::InputText("Expansion URL", expansionUrl, 500);
		if(ImGui::Button("Reload Pictures")) {
			reloadPictures();
		}
		ImGui::SameLine();
		ImGui::Text("(Will probably freeze as image loads)");
		if(ImGui::Button("Choose From Official Images")) {
			ImGui::OpenPopup("Choose from Official Images");
		}
		if(cardLayout <= 2) if(ImGui::Button("Choose Official Expansion Icon")) {
			ImGui::OpenPopup("Choose Official Expansion Icon");
		}
		ImGui::SliderFloat("X Move Distance", &xMove, -1.f, 1.f, "%.3f");
		ImGui::SliderFloat("Y Move Distance", &yMove, -1.f, 1.f, "%.3f");
		ImGui::SliderFloat("Zoom", &zoom, 0.25f, 4.f, "%.2f");
		if(ImGui::Button("Reset Positions")) {
			xMove = 0;
			yMove = 0;
			zoom  = 1;
		}
		
		if(ImGui::Button("Save Screenshot")) 	screenShot();
		
		if(ImGui::Button("Copy to Clipboard")) 	copyToClipboard();
		
		if (ImGui::Button("Click To Reset All")) ImGui::OpenPopup("Reset All");

		if(ImGui::BeginPopupModal("Load Example", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::ListBox("Official Icons", &exampleSelected, examplesNames, IM_ARRAYSIZE(examplesNames), 15);
			if(ImGui::Button("Load Example")) {
				Saves::read(string(examplesUrls[exampleSelected]));
				reloadPictures();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if(ImGui::BeginPopupModal("Reset All", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Are you sure you want to delete everything?\nThis will restart the program.");
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				int pid = fork();
				if(pid == 0) {
					execv("./DCLP", (char *const[]){(char*)"./DCLP"});
				} else {
					glfwHideWindow(window);
					system("rm ./save.dclp ./tempicon.png expansionicon.png > /dev/null");
					waitpid(pid, 0, 0);
					exit(0);
				}
			}
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
		}
		if(ImGui::BeginPopupModal("Choose from Official Images", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			if(ImGui::InputText("Search (CASE SENSITIVE)", artSearch, 100) || isFirstFrame) {
				int i = 0;
				p = 0;
				for(; i < sizeof(artworkNames) / sizeof(char*); i++) {
					if(strstr(artworkNames[i], trimwhitespace(artSearch)) != nullptr) {
						shownArtworks[p++] = artworkNames[i];
					}
				}
				shownArtworks[p] = NULL;
			}

			ImGui::ListBox("Official Images", &imageToLoad, shownArtworks, p, 15);

			if(ImGui::Button("Load Selected Image")) {
				int pos = 0;
				for(int i = 0; i < IM_ARRAYSIZE(artworkNames); i++) {
					if(strcmp(shownArtworks[imageToLoad], artworkNames[i]) == 0) {
						pos = i;
						break;
					}
				}
				for(int i = 0; ; i++) {
					iconUrl[i] = artworkUrls[pos][i];
					if(artworkUrls[pos][i] == 0) break;
				}
				reloadPictures();
				ImGui::CloseCurrentPopup();
			}

			if(ImGui::Button("Close Window")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		if(ImGui::BeginPopupModal("Choose Official Expansion Icon", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::ListBox("Official Icons", &imageToLoad, expansionIconList, IM_ARRAYSIZE(expansionIconList), 15);

			if(ImGui::Button("Load Selected Image")) {
				string tem = string(expansionIconListImage[imageToLoad]);
				string exePath = getPathToPwd();
				string toLoadS = "file://" + exePath + "/" + tem;
				cout << toLoadS << endl;
				int i = 0;
				for(; i < toLoadS.size(); i++) {
					expansionUrl[i] = toLoadS.at(i);
				}
				expansionUrl[i] = 0;
				reloadPictures();
				ImGui::CloseCurrentPopup();
			}

			if(ImGui::Button("Close Window")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		
		if(cardLayout != 3) ImGui::Text("Tweaks");
		if(cardLayout <= 2) ImGui::Checkbox("Large Single Line Vanilla Bonuses", &largeSingleLineVanillaBonuses);
		if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text Border Width", &textXTweak, 0.3f, 4.f, "%.2f");
		if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text X Position", &textXPosTweak, -1.f, 1.f, "%.2f");
		if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text Y Position", &textYPosTweak, -1.f, 1.f, "%.2f");
		if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text Size", &textSizeTweak, 0.3f, 4.f, "%.2f");
		if(cardLayout <= 2) ImGui::SliderFloat("Tweak Vanilla Bonus Size", &bonusSizeTweak, 0.3f, 4.f, "%.2f");

		if(cardLayout != 3) ImGui::SliderFloat("Tweak Expansion Icon X Size", &expansionIconXSizeTweak, 0.5f, 2.f, "%.2f");
		if(cardLayout != 3) ImGui::SliderFloat("Tweak Expansion Icon Y Size", &expansionIconYSizeTweak, 0.5f, 2.f, "%.2f");

		if(cardLayout <= 1) ImGui::SliderFloat("Tweak Dividing Line Y Position", &tweakDividingLineY, -1.f, 1.f, "%.2f");
		if(cardLayout <= 2) ImGui::SliderFloat("Bottom Text Size Tweak", &bottomTextSizeTweak, 0.3f, 2.f, "%.2f");

    	ImGui::End();

		
    	// ImGui::ShowDemoWindow();
    }
	
}
static void draw() {
    handlerDraw();
	
	composeDearImGuiFrame();
    ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}
static void changeFullscreen() {
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
static void key_callback(GLFWwindow* eventWindow, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_S && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
	   Log::log("Ctrl+Alt+S pressed, segfaulting. This will kill everything in its place, probably causing things like data loss and stuff.");
	   Log::flushFile();
		
       raise(11);
    }
    if(key == GLFW_KEY_F4 && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
	shouldRedraw = true;
    if(action != GLFW_PRESS) {
        return;
    }
	if(isPausedMaster) update();
	if(key == GLFW_KEY_F4 && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		isPausedMaster = !isPausedMaster;
		Log::debug(string(isPausedMaster ? "Pausing " : "Unpausing ") + "game");
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
static void window_size_callback(GLFWwindow* window, int width, int height) {
    Log::debug("Window resized to " + doubleToString(width) + "x" + doubleToString(height));
	setFloat("pixelWidth", pixelWidth);
	setFloat("pixelHeight", pixelHeight);
	shouldRedraw = true;
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(action == GLFW_PRESS) {
		onMouse(getX(), getY(), action, button);
	}
	if(action == GLFW_RELEASE) onMouseRelease();
	shouldRedraw = true;
}
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	shouldRedraw = true;
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	onScroll(xoffset, yoffset);
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
void APIENTRY errorPassThrough(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam) {
	if(hasDebugMode) {
		cout << message << endl;
	}
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
	
	e = glfwCreateWindow(v->width, v->height, "Bang", NULL, NULL);
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
void resetMatrix() {
	int iwidth, iheight;
	glfwGetFramebufferSize(window, &iwidth, &iheight);
	windowRatio = (float) iwidth / (float) iheight;
	windowInverse = (float)iheight / (float)iwidth;
	glViewport(0, 0, iwidth, iheight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-getHBorder(), getHBorder(), -getYBorder(), getYBorder(), 1.f, -1.f);
	//glOrtho(-baseWindowRatio, baseWindowRatio, -baseWindowRatio, baseWindowRatio, 1.f, -1.f);

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
	
	if(screenshottingNextFrame) {
		setMat4("transMat", glm::scale(glm::vec3(-1.f, 1.f, 1.f)));
	} else {
		setMat4("transMat", glm::mat4(1.f));
	}
	
	glUseProgram(shaderProgram);
}
void imguiInit() {
	// https://marcelfischer.eu/blog/2019/imgui-in-sdl-opengl/
	
	IMGUI_CHECKVERSION();
	
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	ImGui::StyleColorsDark();
	
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}
bool isScreenshotting = false;
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
void printColors() {
	for(int i = 0; i < 18; i++) {
		cout << i+1 << "." << (i < 9 ? "  " : " ") << mainChoices[i] << endl;
	}
}
void printLegend() {
	cout << "--- LEGEND ---" << endl <<
			"$\tMoney symbol (Ya know, the coin)" << endl <<
			"@\tDebt symbol" << endl <<
			"^\tPotion symbol" << endl <<
			"%\tVictory point symbol" << endl;
}
void printLayouts() {
	cout << "--- Dominion Card Layouts ---" << endl <<
			"0\tNormal type of card. Used by Actions, Reactions, etc." << endl <<
			"1\tLandscape/horizontal card. Used by Events, Projects, etc." << endl <<
			"2\tBase card. Used by Copper, Silver, Estate, Province, Curse, etc." << endl <<
			"";
}
int doSingleArgument(string arg, string next, int* i) { // Trust me on this one
	
	return 0;
}
string ReplaceAll(string str, const string& from, const string& to) { // https://stackoverflow.com/a/24315631
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}
int main(int argc, char *argv[]) {
	showWindow = false;
	bool isCli = false;
	for(int i = 1; i < argc; i++) { // Lengthy, but it works, so I don't care
		if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			printHelp();
			return 0;
		} else if(strcmp(argv[i], "--high-res") == 0) {
			cout << "Using higher resolution images, expect lower performance and longer load times." << endl;
			isLowRes = false;
		} else if(strcmp(argv[i], "--title") == 0) {
			incrementLoop();
			cardTitle = argv[i];
			cout << "Using \"" << cardTitle << "\" as title." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--type") == 0) {
			incrementLoop();
			cardType = argv[i];
			cout << "Using \"" << cardType << "\" as type." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--cost") == 0) {
			incrementLoop();
			cardCost = argv[i];
			cout << "Using \"" << cardCost << "\" as cost." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--url") == 0) {
			incrementLoop();
			iconUrl = argv[i];
			cout << "Loading \"" << iconUrl << "\" as image." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--expansion-url") == 0) {
			incrementLoop();
			expansionUrl = argv[i];
			cout << "Loading \"" << expansionUrl << "\" as expansion icon." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--primary-color") == 0) {
			incrementLoop();
			cardColor = getChoice(argv[i]);
			cout << "Using \"" << mainChoices[cardColor] << "\" as primary color." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--secondary-color") == 0) {
			incrementLoop();
			cardSecondary = getChoice(argv[i])+1;
			cout << cardSecondary << endl;
			cout << "Using \"" << secondaryChoices[cardSecondary] << "\" as secondary color." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--text") == 0) {
			incrementLoop();
			cardText = argv[i];
			cout << "Card text acquired." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--illustration-credit") == 0) {
			incrementLoop();
			cardCredit = argv[i];
			cout << "Using \"" << cardCredit << "\" as illustration credit." << endl;
			
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--author") == 0) {
			incrementLoop();
			cardVersion = argv[i];
			cout << "Using \"" << cardVersion << "\" as version and author credit." << endl;
			
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--preview") == 0) {
			incrementLoop();
			cardPreview = argv[i];
			cout << "Using \"" << cardVersion << "\" as preview." << endl;
			
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--legend") == 0) {
			printLegend();
			return 0;
		} else if(strcmp(argv[i], "--font-size") == 0) {
			incrementLoop();
			fontSize = stoi(argv[i]);
			if(fontSize == 0) {
				Log::warning("Font size was zero or unrecognisable, resetting to 144.");
				fontSize = 144;
			}
			if(fontSize > 500) {
				Log::warning("Font size is over 500! Expect longer render and load times.");
			}
		} else if(strcmp(argv[i], "--curl-license") == 0) {
			printCurlLicense();
			return 0;
		} else if(strcmp(argv[i], "--list-colors") == 0) {
			printColors();
			return 0;
		} else if(strcmp(argv[i], "--list-types") == 0) {
			printColors();
			return 0;
		}
	}
	
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
	
	vector<string> splitted = split(getPathToExe(), "/");
	pathPrefix = "./";
	for(int i = 0; i < splitted.size(); i++) {
		//pathPrefix += splitted[i] + "/";
	}
	Log::log("Path prefix should be " + pathPrefix);
	
	int logReturn = Log::loggerInit();
	if(logReturn != 0) {
		cout << "Logger init failed!" << endl;
	}
	
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    const GLFWvidmode* v = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window = createWindow(false);
    

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize OpenGL context! Exiting" << std::endl;
        return -1;
    }
	
	if(!isScreenshotting) imguiInit();
	
   	textInit();
	loadFont("cinzel.ttf", "trajan");
	loadFont("cinzel-bold.ttf", "trajanb");
	loadFont("tnr-bold.ttf", "tnrb");
	loadFont("tnr.ttf", "tnr");
	loadFont("tnri.ttf", "tnri");
	loadFont("tnri.ttf", "tnrib");
	
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	
   	res::initial();
	
	utilsSetup();
	
	textInit();
	
	handOnLoad();

	Saves::init();
	Saves::read();

	loadIcon(string(iconUrl), "./tempicon.png", &(res::tempIcon), true);
	loadIcon(string(expansionUrl), "./expansionicon.png", &(res::tempExpansionIcon), true);
	
	glfwSwapInterval(1);

	//changeFullscreen();
	
	float deltaFloat = 0.f;
	
	if(isScreenshotting) {
		loadIcon(string(iconUrl), "./tempicon.png", &(res::tempIcon), false);
		loadIcon(string(expansionUrl), "./expansionicon.png", &(res::tempExpansionIcon), false);
		calculateFPS();
      enablings();
		for(int i = 0; i < 10; i++) {
			resetMatrix();
			update();
			handlerDraw();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		screenShot();
		return 0;
	}
	
	glfwShowWindow(window);
	
    while (!glfwWindowShouldClose(window))
    {
		/*if(!isWindowFocused) {
			glfwPollEvents();
			continue;
		}*/
        calculateFPS();
        enablings();
		
		resetMatrix();
		
		deltaFloat = glfwGetTime();
        if(!isPausedMaster) {
			update();
			updateDelta = (glfwGetTime()-deltaFloat)*1000;
			updateDeltaAdd += updateDelta;
		} else {
			updateDelta = 0.f;
		}
		
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
	
	bangExit();
    return 0;
}

#ifdef _WIN64
int WinMain() {
	main(0, nullptr);
}
#endif
