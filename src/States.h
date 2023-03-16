#pragma once
//#include <glad/glad.h>

#include "utils.h"
#include "main.h"
#include <string>
#include "Resources.h"

using namespace std;

class State {
    public:
        virtual void update();
		virtual void unfixedUpdate() {};
        virtual void draw();
        virtual void onKeyPress(int key, int action, int mods);
        virtual void onMouseClick(double x, double y, int action, int button);
		virtual void onMouseHold(double x, double y) {};
		virtual void onMouseRelease(double x, double y) {};
		virtual string getDebugMenu() {return State::getBaseDebugMenu();};
		static  string getBaseDebugMenu() {
			return "Average FPS: " + to_string(currentFps) + "\nX: " + to_string(getX()) + "\nY: " + to_string(getY());
		};
		virtual void postProcessing() {};
		virtual void onLoad() {};
		virtual void onFirstLoad() {};
		virtual void onUnload() {};
		virtual void onScrollEvent(float xOff, float yOff) {};
		virtual ~State() {};
        State();
};

/*
base state template
class BaseState: public State {
   public:
      void update() override;
      void draw() override;
      void onKeyPress(int key, int action, int mods) override;
      void onMouseClick(double x, double y, int action, int button) override;
};
*/

class MenuState: public State {
    public:
        float hrot;
        float rot;
        float rotvel;
        bool goingup;
        Point2 points[4] = {{-1.f, -0.2f},{1.f, -0.2f},{1.f, -0.5f},{-1.f, -0.5f}};
        void update() override;
        void draw() override;
        void onKeyPress(int key, int action, int mods) override;
        void onMouseClick(double x, double y, int action, int button) override;
};

class TestState: public State {
    public:
        bool isGreen = true;
        void update() override;
        void draw() override;
        void onKeyPress(int key, int action, int mods) override;
        void onMouseClick(double x, double y, int action, int button) override;
};
