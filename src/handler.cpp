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

// This file was more important in a separate project which had lots of functionality that was not needed for this. 
// The entire file is not strictly needed, but I don't want to do the work to remove it.

MainDrawState* t;

bool isMouseDown = false;

void handOnLoad() {
	delete t;
	t = new MainDrawState();
}

void handlerUpdate() {
    t->update();
}

void onKey(int key, int action, int mods) {
	t->onKeyPress(key, action, mods);
}

void onMouse(double x, double y, int action, int button) {
    t->onMouseClick(x, y, action, button);
	isMouseDown = true;
}

void onMouseRelease() {
	isMouseDown = false;
}

void handlerDraw() {
	setTint(1.f, 1.f, 1.f);
	setFloat("alpha", 1.f);
	
    t->draw();
}