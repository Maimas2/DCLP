#include "Image.h"
#include "States.h"

extern double TRANSITION_TYPE;
#define TRANSITION_NUMBER 4

#define TRANSITION_FADE      0
#define TRANSITION_SLIDE     1
#define TRANSITION_ZOOM      2
#define TRANSITION_TAKE      3

void handOnLoad();

void handlerUnfixedUpdate();
void handlerUpdate();

void onKey(int key, int action, int mods);
void onMouse(double x, double y, int action, int button);
void onMouseRelease();
void onScroll(float xo, float yo);
string getTransitionName(double in);

void handlerDraw();

void goToState(State* newState);

void exitToParent();

void changeTransitionType(double newType);
