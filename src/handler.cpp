#ifndef GL_IS_INCLUDED
#define GL_IS_INCLUDED
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif
#include <iostream>
#include <vector>
#include <map>

#include "utils.h"
#include "handler.h"
#include "Image.h"
#include "States.h"
#include "main.h"
#include "Text.h"
#include "Log.h"
#include "Resources.h"

using namespace std;

bool isDebugMenu = true;

double TRANSITION_TYPE = 0.f;

bool loaded = false;
State* t;
State* goingTo;
State* sstack[16];
int currentPlace = -1;
bool transitioning = false;
float trans = 0.f;
bool goingUp = true;
bool isPoppingBack = false;

bool isMouseDown = false;

void checkStackValidity() {
	if(currentPlace >= 16) {
		Log::fatal("STATE STACK OVERFLOW ERROR.\nThis is due to too many State objects being added to the stack, stop doing that.", true);
		return;
	} else if(currentPlace < 0) {
		Log::fatal("STATE STACK UNDERFLOW ERROR.\nThis is due to not enough State objects being in the stack, \
		you should not be seeing this without some wonky glitches.", true);
		return;
	}
}
map<double, string> transitionNames;
void addTransitionNames() {
	transitionNames.clear();
	
	transitionNames[0] = "Fade";
	transitionNames[1] = "Slide";
	transitionNames[2] = "Zoom";
	transitionNames[3] = "Take";
}

string getTransitionName(double in) {
	string toReturn = transitionNames[in];
	if(toReturn == "") return doubleToString(in);
	return toReturn;
}

void checkLoadedness() {
	if(currentPlace < 0) {
		handOnLoad();
	}
}

void push_back(State* r) {
	currentPlace++;
	checkStackValidity();
	sstack[currentPlace] = r;
	t = sstack[currentPlace];
}

void pop_back() {
	delete sstack[currentPlace];
	currentPlace--;
	checkStackValidity();
	t = sstack[currentPlace];
}

State* back() {
	return sstack[currentPlace];
}

//Ok, so you call this function to actually change states and call all apropriate functions.
void actuallyChangeStateTransition() {
	t->onUnload();
	if(isPoppingBack) {
		isPoppingBack = false;
		pop_back();
		trans = 1.f;
		goingUp = false;
		goingTo = NULL;
		t = back();
		t->onLoad();
	} else {
		push_back(goingTo);
		trans = 1.f;
		goingUp = false;
		goingTo = NULL;
		t = back();
		Log::debug("Switching state.");
		t->onFirstLoad();
		t->onLoad();
	}
}

void startTransition() {
	/*t->onUnload();
	if(isPoppingBack) {
		pop_back();
	} else {
		push_back(goingTo);
	}
	t->onLoad();*/
	if(TRANSITION_TYPE == TRANSITION_TAKE) {
		actuallyChangeStateTransition();
		return;
	}
	transitioning = true;
	goingUp = true;
}

void goToState(State* newState) {
    if(transitioning) {
        return;
    }
    goingTo = newState;
    startTransition();
}

void exitToParent() {
    if(transitioning) {
        return;
    }
    //pop_back();
	isPoppingBack = true;
    goingTo = back();
    startTransition();
}

void handOnLoad() {
	delete goingTo;
	delete t;
	addTransitionNames();
	t = new TestState();
	push_back(t);
	t->onLoad();
	loaded = true;
	currentPlace = 0;
}

void handlerUnfixedUpdate() {
	checkLoadedness();
	t->unfixedUpdate();
}

void handlerUpdate() {
    checkLoadedness();
	bg::updateAllPoints();
    if(transitioning) {
        if(goingUp) {
            if(trans >= 1.f) {
				actuallyChangeStateTransition();
            } else {
                trans += 1.f/20.f;
            }
        } else {
            if(trans < 0.f) {
                transitioning = false;
                trans = 0.f;
            } else {
                trans -= 1.f/20.f;
            }
        }
    }
    t->update();
	if(isMouseDown) {
		t->onMouseHold(getX(), getY());
	}
}

void onKey(int key, int action, int mods) {
	checkLoadedness();
	if(key == GLFW_KEY_D && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		isDebugMenu = !isDebugMenu;
		Log::debug("Debug Menu enabled is set to " + to_string(isDebugMenu));
	}
    t->onKeyPress(key, action, mods);
}

void onMouse(double x, double y, int action, int button) {
	checkLoadedness();
    t->onMouseClick(x, y, action, button);
	isMouseDown = true;
}

void onMouseRelease() {
	isMouseDown = false;
}

void onScroll(float xo, float yo) {
	t->onScrollEvent(xo, yo);
}

void handlerDraw() {
	checkLoadedness();
	
	setTint(1.f, 1.f, 1.f);
	setFloat("alpha", 1.f);
	
	if(transitioning) {
		if(TRANSITION_TYPE == TRANSITION_SLIDE) {
			glViewport(0, -(int)(trans*pixelHeight), (GLsizei)pixelWidth, (GLsizei)pixelHeight);
		} else if(TRANSITION_TYPE == TRANSITION_ZOOM) {
			int width = (trans/2) * pixelWidth;
			int height = (trans/2) * pixelHeight;
			glViewport(width, height, (GLsizei)pixelWidth-width*2, (GLsizei)pixelHeight-height*2);
		}
	}
	
    //Draw scene
    t->draw();
	
    //resetMatrix();

    //draw transition fade
    if(transitioning) {
		if(TRANSITION_TYPE == TRANSITION_SLIDE) {
			
		} else if(TRANSITION_TYPE == TRANSITION_FADE) {
			setAlpha(trans);
			setInt("oobAllowed", 1);
			glDisable(GL_DEPTH_TEST);
			drawColoredQuad(-xStretch, -yStretch, xStretch*2, yStretch*2, allBlack);
			setAlpha(1.f);
			setInt("oobAllowed", 0);
			glEnable(GL_DEPTH_TEST);
		}
    }
	
	if(isDebugMenu) {
		glDisable(GL_DEPTH_TEST);
		isDrawingLargeIcons = false;
		drawString(t->getDebugMenu(), -(16.f/9.f), 1.f, 1.f, 1.f, 1.f, 0.f);
		isDrawingLargeIcons = true;
		glEnable(GL_DEPTH_TEST);
	}
	
	t->postProcessing();
}

void changeTransitionType(double newType) {
	TRANSITION_TYPE = (double)((int)newType % TRANSITION_NUMBER);
}
