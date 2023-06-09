#ifndef GL_IS_INCLUDED
#define GL_IS_INCLUDED
//#include <curl/easy.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stb_image.h>
#include <cmath>
#include <math.h>
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <glm/gtc/type_ptr.hpp>

#include "utils.h"
#include "Image.h"
#include "main.h"
#include "Resources.h"
#include "Log.h"
#include "Text.h"
#include "Saves.h"

#include "curl/curl.h"

using namespace std;

int vShader;
int fShader;
int shaderProgram;
int vao;
float currentAlpha[4] = {1.f, 1.f, 1.f, 1.f};
double moneyMadeThisGame = -1;
bool isFixedFunction = false;

float allRed[12] = {
	1.f, 0.f, 0.f,
	1.f, 0.f, 0.f,
	1.f, 0.f, 0.f,
	1.f, 0.f, 0.f,
};

float allGray[12] = {
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
};

float allBlack[12] = {
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f,
	0.f, 0.f, 0.f,
};
float allBlue[12] = {
	0.f, 0.f, 1.f,
	0.f, 0.f, 1.f,
	0.f, 0.f, 1.f,
	0.f, 0.f, 1.f,
};
float allWhite[12] = {
	1.f, 1.f, 1.f,
	1.f, 1.f, 1.f,
	1.f, 1.f, 1.f,
	1.f, 1.f, 1.f,
};
float allGreen[12] = {
	0.f, 1.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 1.f, 0.f,
	0.f, 1.f, 0.f,
};
float allYellow[12] = {
	1.f, 1.f, 0.f,
	1.f, 1.f, 0.f,
	1.f, 1.f, 0.f,
	1.f, 1.f, 0.f,
};
float allBrown[12] = {
	1.2f, 0.8f, 0.5f,
	1.2f, 0.8f, 0.5f,
	1.2f, 0.8f, 0.5f,
	1.2f, 0.8f, 0.5f,
};
float allDarkGray[12] = {
	0.25f, 0.25f, 0.25f,
	0.25f, 0.25f, 0.25f,
	0.25f, 0.25f, 0.25f,
	0.25f, 0.25f, 0.25f,
};
float eventColor[12] = {
	0.9f, 0.8f, 0.7f,
	0.9f, 0.8f, 0.7f,
	0.9f, 0.8f, 0.7f,
	0.9f, 0.8f, 0.7f,
};

float tintR = 1.f, tintG = 1.f, tintB = 1.f;

float degreesToRadians(float degree) {
	return ((degree * 3.14159) / 180.f);
}

double getMouseX() {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return xpos;
}

double getMouseY() {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return ypos;
}

double getxPos(double xPos) {
	//return ((xPos-(pixelWidth/2))/(pixelWidth/2))*min(xStretch, (16.f/9.f));
	return ((xPos-(pixelWidth/2))/(pixelWidth/2));
}

double getyPos(double yPos) {
	return ((yPos-(pixelHeight/2))/(pixelHeight/2))*min(yStretch, (16.f/9.f));
}

double getX() {
	return getxPos(getMouseX())*xStretch;
}

double getY() {
	return (-getyPos(getMouseY())/(windowRatio >= (16.f/9.f) ? 1.f : (16.f/9.f)))*yStretch;
	//return (-getyPos(getMouseY()))*yStretch;
}

void checkShader(int shader) {
	int success;
	char returnLog[300];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(shader, 300, NULL, returnLog);
		Log::error("Shader compilation failed! Dropping into fixed function pipeline! This will most surely cause rendering issues/failure. Here is the error:\n\t\t\t\t\t\t\t" + string(returnLog));
	}
}

void setBool(string name, bool value) {
	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
}
void setInt(string name, int value) {
	glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
void setFloat(string name, float value) {
	glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
void setVec2(string name, float value1, float value2) {
	glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), value1, value2);
}
void setVec3(string name, float value1, float value2, float value3) {
	glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), value1, value2, value3);
}
void setVec4(string name, float value1, float value2, float value3, float value4) {
	glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), value1, value2, value3, value4);
}
void setMat4(string name, glm::mat4 in) {
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(in));
}
int shaderSource(string vertexCode, string fragmentCode) {
	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//glShaderSource(vertexShader, 1, (const GLchar* const*)readFile((char*)"shaders/vertex.vs"), NULL);
	glCompileShader(vertexShader);
	checkShader(vertexShader);
	vShader = vertexShader;

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//glShaderSource(fragmentShader, 1, (const GLchar* const*)readFile((char*)"shaders/fragment.fs"), NULL);
	glCompileShader(fragmentShader);
	checkShader(fragmentShader);
	fShader = fragmentShader;

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);

	return shaderProgram;
}
unsigned char* loadImageToChar(string source, bool hasTrans, int &width, int &height, int &nrChannels) {
	return stbi_load(source.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
}
ImageStruct loadImageToInt(string source, bool hasTrans) {
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char* data = loadImageToChar(source, hasTrans, width, height, nrChannels);
	if (data)
	{
		if(hasTrans) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		} else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,  width, height, 0, GL_RGB,  GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		Log::warning("Failed to load texture " + source + ", expect rendering issues.");
		return {0, 0, 0};
	}
	stbi_image_free(data);
	return {width, height, texture};
}
Image loadImage(string source, bool hasTrans) {
	Image i;
	//i.id = c.textureId;
	i.path = source;
	i.doIHaveTrans = hasTrans;
	i.load();
	return i;
}
void clear(float r, float g, float b) {
	/*glClearColor(r * tintR, g * tintG, b * tintB, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/
	float l[] = {
		r*tintR, g*tintG, b*tintB,
		r*tintR, g*tintG, b*tintB,
		r*tintR, g*tintG, b*tintB,
		r*tintR, g*tintG, b*tintB
	};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setInt("oobAllowed", 1);
	drawColoredQuad(-xStretch, -yStretch, 5.1f, xStretch*2, yStretch*2, l);
	setInt("oobAllowed", 0);
}
Point2 rotatePoint(Point2 p, float ang, float cx, float cy) {
	float angle = degreesToRadians(ang);
	return {cos(angle) * (p.x - cx) - sin(angle) * (p.y - cy) + cx,
			sin(angle) * (p.x - cx) + cos(angle) * (p.y - cy) + cy};
}
void plotPoint(Point2 in) {
	glVertex2f(in.x, in.y);
}
void fillOval(float x, float y, float z, float width, float height) {
	res::circle.bind();
	drawTexturedQuad(x, y, z, width, height);
}
void doVertices(float v[32]) {
	/*glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v), v);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/
}
void baseQuadDraw(Point2 p[4], float z, float rgb[12]) {
	/*float vertices[] = {
	   // positions              // colors                       // texture coords
		p[0].x,   p[0].y, z,   rgb[0], rgb[1],  rgb[2],    1.0f, 0.0f, currentAlpha[0], // top right
		p[1].x,   p[1].y, z,   rgb[3], rgb[4],  rgb[5],    1.0f, 1.0f, currentAlpha[1], // bottom right
		p[2].x,   p[2].y, z,   rgb[6], rgb[7],  rgb[8],    0.0f, 1.0f, currentAlpha[2], // bottom left
		p[3].x,   p[3].y, z,   rgb[9], rgb[10], rgb[11],   0.0f, 0.0f, currentAlpha[3], // top left 
	};*/
	if(isFixedFunction) {
		glBegin(GL_QUADS);
		
		glColor4f(rgb[0], rgb[1], rgb[2], currentAlpha[0]);
		glTexCoord2f(0.f, 1.f);
		glVertex3f(p[0].x, p[0].y, z);
		
		
		glColor4f(rgb[3], rgb[4], rgb[5], currentAlpha[1]);
		glTexCoord2f(1.f, 1.f);
		glVertex3f(p[1].x, p[1].y, z);
		
		
		glColor4f(rgb[6], rgb[7], rgb[8], currentAlpha[2]);
		glTexCoord2f(1.f, 0.f);
		glVertex3f(p[2].x, p[2].y, z);
		
		
		glColor4f(rgb[9], rgb[10], rgb[11], currentAlpha[3]);
		glTexCoord2f(0.f, 0.f);
		glVertex3f(p[3].x, p[3].y, z);
		
		glEnd();
		
		return;
	}
	int i = 0;
	float vertices[] = {
	   // positions              // colors                       // texture coords
		p[0].x,   p[0].y, z,   rgb[0], rgb[1],  rgb[2],    0.0f, 1.0f, currentAlpha[0], // top right
		p[1].x,   p[1].y, z,   rgb[3], rgb[4],  rgb[5],    1.0f, 1.0f, currentAlpha[1], // bottom right
		p[2].x,   p[2].y, z,   rgb[6], rgb[7],  rgb[8],    1.0f, 0.0f, currentAlpha[2], // bottom left
		p[3].x,   p[3].y, z,   rgb[9], rgb[10], rgb[11],   0.0f, 0.0f, currentAlpha[3], // top left 
	};
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
}
void drawTexturedQuad(float x, float y, float z, float w, float h) {
	float c[] = {
		tintR, tintG, tintB,
		tintR, tintG, tintB,
		tintR, tintG, tintB,
		tintR, tintG, tintB
	};
	drawColoredTexture(x, y, z, w, h, c);
}
void drawTexturedQuad(float x, float y, float w, float h) {
	drawTexturedQuad(x, y, 0.f, w, h);
}
void drawColoredTexture(float x, float y, float z, float wid, float hei, float rgb[12]) {
	Point2 p[4] = {
		{x, y},
		{x+wid, y},
		{x+wid, y+hei},
		{x, y+hei}
	};
	baseQuadDraw(p, z, rgb);
}
void drawColoredTexture(float x, float y, float wid, float hei, float rgb[12]) {
	drawColoredTexture(x, y, 0.f, wid, hei, rgb);
}
void setTint(float r, float g, float b) {
	tintR = r;
	tintG = g;
	tintB = b;
}
void drawColoredQuad(float x, float y, float z, float wid, float hei, float rgb[12]) {
	res::allWhite.bind();
	Point2 p[4] = {
		{x, y},
		{x+wid, y},
		{x+wid, y+hei},
		{x, y+hei}
	};
	baseQuadDraw(p, z, rgb);
}
void drawColoredQuad(float x, float y, float wid, float hei, float rgb[12]) {
	drawColoredQuad(x, y, 0.f, wid, hei, rgb);
}
void plotPoints(Point2 in[4], float z) {
	baseQuadDraw(in, z, allWhite);
}
void plotPoints(Point2 in[4]) {
	plotPoints(in, 0.f);
}
string doubleToString(double d, int precision) {
	stringstream stream;
	stream << fixed << setprecision(precision) << d;
	return stream.str();
}
string doubleToString(double d) {
	return doubleToString(d, 0);
}
void expansionExit();
void dclpExit() {
	expansionExit();

	Saves::exit();

	Log::log("Terminating saves and logging services, then terminating GLFW.");

	Log::loggerExit();
	
	glfwDestroyWindow(window);
	glfwTerminate();
	
	exit(EXIT_SUCCESS);
}
void setupVertexPointers(int size) {
	GLsizei stride = size*sizeof(float);
	
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}
int setupVAO() {
   /*float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f, currentAlpha,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f, currentAlpha,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f, currentAlpha,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f, currentAlpha,   0.0f, 1.0f, // top left 
    };*/
	float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, currentAlpha[0], // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, currentAlpha[1], // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, currentAlpha[2], // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f, currentAlpha[3], // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
	
	setupVertexPointers(9);
    
    return EBO;
}
void utilsSetup() {
	vao = setupVAO();
	shaderProgram = shaderSource(" \n\
#version 130 \n\
//layout (location = 0) in vec3 aPos; \n\
//layout (location = 1) in vec4 aColor; \n\
//layout (location = 2) in vec3 aTexCoord; \n\
in vec3 bPos; \n\
in vec4 aColor; \n\
in vec3 aTexCoord; \n\
 \n\
out vec3 color; \n\
out vec2 TexCoord; \n\
out vec3 pos; \n\
out float alpha; \n\
 \n\
uniform float xStretch; \n\
uniform float yStretch; \n\
uniform mat4  transMat; \n\
uniform mat4  baseTransMat; \n\
uniform vec3  translationMat; \n\
 \n\
void main() { \n\
	vec3 aPos = (bPos * mat3(transMat)) * mat3(baseTransMat); \n\
    gl_Position = vec4((aPos.x+translationMat.x)/xStretch, (aPos.y+translationMat.y)/yStretch, ((aPos.z+translationMat.z)+50)/100.11, 1); \n\
    //gl_Position = vec4(aPos.x/xStretch, aPos.y, (aPos.z+50)/100.11, 1) * transMat; \n\
    color = aColor.xyz; \n\
    TexCoord = aTexCoord.xy; \n\
	pos = aPos; \n\
	alpha = aTexCoord.z; \n\
} \n\
	",
	
	"\n\
#version 130 \n\
out vec4 fc; \n\
 \n\
in vec3 color; \n\
in vec2 TexCoord; \n\
in vec3 pos; \n\
in float alpha; \n\
 \n\
uniform sampler2D tex; \n\
uniform float xStretch; \n\
uniform float yStretch; \n\
uniform float pixelWidth; \n\
uniform float pixelHeight; \n\
uniform bool oobAllowed; \n\
uniform bool maxX; \n\
uniform vec2 maxNE; \n\
uniform vec2 maxSW; \n\
 \n\
void main() { \n\
   if(alpha == 0) discard; \n\
   if(maxX && (pos.x > maxNE.x || pos.x < maxSW.x || pos.y > maxNE.y || pos.y < maxSW.y)) discard; \n\
   if((abs(pos.x) > (16.f/9.f) || abs(pos.y) > 1) && !oobAllowed) { \n\
	  discard; \n\
      fc = vec4(0, 0, 0, 0); \n\
	  return; \n\
   } \n\
   vec4 FragColor; \n\
   FragColor = texture(tex, TexCoord); \n\
   fc = FragColor * vec4(color, alpha); \n\
} \n\
	");
}float getRandom() {
	return ((float)rand())/(float)RAND_MAX;
}
void setAlpha(float one, float two, float three, float four) {
	setVec4("alpha", one, two, three, four);
	//currentAlpha = {one, two, three, four};
	currentAlpha[0] = one;
	currentAlpha[1] = two;
	currentAlpha[2] = three;
	currentAlpha[3] = four;
}
void setAlpha(float al) {
	setAlpha(al, al, al, al);
}
void setOob(bool oo) {
	setInt("oobAllowed", (oo ? 1 : 0));
}
char* readFileToChar(string path) {
	FILE* f = fopen(path.c_str(), "r");
	
	char* buffer = (char*)malloc(256);
	auto ignoreMe = fgets(buffer, 256, f);
	
	fclose(f);
	return buffer;
}
void drawRoundedRectangle(float x, float y, float z, float width, float height, float bearingX, float bearingY, float* color) {
	if(height < 0) {
		y += height;
		height = abs(height);
	}
	bearingX = min(bearingX, width/2);
	bearingX = min(bearingY, height/2);
	drawColoredQuad(x, y+bearingY/2, z, width, height-bearingY, color);
	drawColoredQuad(x+bearingX/2, y, z, width-bearingX, height, color);
	res::circle.bind();
	drawColoredTexture(x, y, z, bearingX, bearingY, color);
	drawColoredTexture(x, y+height-bearingY, z, bearingX, bearingY, color);
	drawColoredTexture(x+width-bearingX, y, z, bearingX, bearingY, color);
	drawColoredTexture(x+width-bearingX, y+height-bearingY, z, bearingX, bearingY, color);
}
vector<string> split(string og, string splitter) {
	if(og.find(splitter) == string::npos) {
		vector<string> tor;
		tor.push_back(og);
		return tor;
	}
	string s = og;
	size_t pos = 0;
	string token;
	vector<string> toReturn;
	while ((pos = s.find(splitter)) != string::npos) {
		token = s.substr(0, pos);
		toReturn.push_back(token);
		s.erase(0, pos + splitter.length());
	}
	toReturn.push_back(s);
	return toReturn;
}
string split(string og, string splitter, bool isFirst) {
	if(og.find(splitter) == string::npos) {
		return og;
	}
	int where = og.find(splitter);
	if(isFirst) {
		return og.substr(0, where);
	} else {
		return og.substr(where+1, og.length());
	}
	return "";
}
void loadIcon(string url, string fileOut, Image* toLoad, bool isLoaded) { // IMPORTANT: DOES NOT SUPPORT WINDOWS
	if(isLoaded) {
		*toLoad = res::setupImage(fileOut, true);
		return;
	}
	CURL* image = curl_easy_init();
	FILE* f = fopen(fileOut.c_str(), "wb");
	curl_easy_setopt(image, CURLOPT_URL, url.c_str());
	curl_easy_setopt(image, CURLOPT_WRITEFUNCTION, NULL); 
	curl_easy_setopt(image, CURLOPT_WRITEDATA, f);

	auto result = curl_easy_perform(image);
	if(result) {
		Log::warning("Loading of image " + url + " failed!");
		return;
	}

	curl_easy_cleanup(image);

	fclose(f);

	*toLoad = res::setupImage(fileOut, true);
	return;
	pid_t pid=fork();
    if(pid == 0) {
		
    } else {
        waitpid(pid,0,0);
		*toLoad = res::setupImage(fileOut, true);
    }
}