#pragma once

#include "utils.h"
#include "main.h"
#include <string>
#include "Resources.h"

class MainDrawState {
    public:
        void update();
        void draw();
        void onKeyPress(int key, int action, int mods);
        void onMouseClick(double x, double y, int action, int button);
};
