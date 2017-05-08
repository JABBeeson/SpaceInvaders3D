#define _USE_MATH_DEFINES
#define GLM_FORCE_RADIANS // force glm to use radians
#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>


//importing header files
#include "headers\Includes.h"
#include "headers\Shader.h"
#include "headers\Mesh.h"
#include "headers\Model.h"
#include "headers\CubeMap.h"
#include "headers\GameWorld.h"

using namespace std;
using namespace chrono;

SDL_Window *win = nullptr;
SDL_GLContext glcontext;
const GLuint SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
bool running = true, fulScrTog = false;
glm::mat4 projectionMatrix, viewMatrix;
vec3 camPos = vec3(0.0f, 0.0f, -20.0f);
vec3 camTarget = vec3(0.0f, 0.0f, 0.0f);
Vector3 viewPos, viewRot;

//support functions
void toggleFullScreen();
void setWindowSize();
high_resolution_clock::time_point getCurrentTime();
char* getFullPath(char* path);
//main Functions
void processInput();
void update(double &deltaTime);
void render(glm::mat4 &projectionMatrix, Shader modelShader, Shader cubeShader);

//Model enemyModel;
GameWorld gameWorld;
CubeMap cubeMap;

void toggleFullScreen() {
	Uint32 FullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	bool fulScrTog = SDL_GetWindowFlags(win) & FullscreenFlag;
	SDL_SetWindowFullscreen(win, fulScrTog ? 0 : FullscreenFlag);
}

void setWindowSize() {
	int w, h, newWidth;
	w = SDL_GetWindowSurface(win)->w;
	h = SDL_GetWindowSurface(win)->h;
	newWidth = (h / 3.0f) * 4.0f;

	glViewport((w - newWidth) / 2, 0.0f, newWidth, h);
}

chrono::high_resolution_clock::time_point getCurrentTime()
{
	return chrono::high_resolution_clock::now();
}

char* getFullPath(char* path) {
	char* fullPath = SDL_GetBasePath();
	strcat(fullPath, path);
	return fullPath;
}

void SetUp() {
	
	vector<const GLchar*> texturePaths{
		getFullPath("assets/Textures/right.jpg"),
		getFullPath("assets/Textures/left.jpg"),
		getFullPath("assets/Textures/top.jpg"),
		getFullPath("assets/Textures/bottom.jpg"),
		getFullPath("assets/Textures/back.jpg"),
		getFullPath("assets/Textures/front.jpg")
	};
	cubeMap = CubeMap(texturePaths);

	Model enemyModel(getFullPath("assets/Models/cardbox.obj"), getFullPath("assets/Textures/vader.png"));
	enemyModel.Scale(0.0005f);

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 3; j++) {
			// create alien 
			Model e = enemyModel;
			//positon them
			e.Move(gameWorld.startPosX + i * gameWorld.Xgap, gameWorld.startPosY + j * gameWorld.Ygap, 0.0f);
			gameWorld.enemies.push_back(e);
			//delete e;
		}
	}
	gameWorld.player = Model(getFullPath("assets/Models/cardbox.obj"), getFullPath("assets/Textures/alienTex.png"));
	gameWorld.player.Scale(0.001f);
	gameWorld.player.Rotate(90.0f, 150.0f, -10.0f);
	gameWorld.player.Move(0.0f, -gameWorld.startPosY, 0.0f);
	//gameWorld.player(getFullPath("assets/Models/cardbox.obj"), getFullPath("assets/Textures/alienTex.png"));

	
}

int main(int argc, char *argv[]) {
	// SDL initialise
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init Error: %s\n",
			SDL_GetError());
		return 1;
	}
	SDL_Log("SDL initialised OK!\n");

	// SDL_Image initialise
	int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL_image init error: %s\n",
			IMG_GetError());
		return 1;
	}
	SDL_Log("SDL_image initialised OK!\n");

	//display Info
	SDL_DisplayMode display;
	SDL_GetCurrentDisplayMode(0, &display);
	int w = display.w, h = display.h;

	// Window Creation
	win = nullptr;
	win = SDL_CreateWindow("Joseph Beeson - 15564479 : Graphics : 3D SpaceInvaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w/2, h/2, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (win == nullptr) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			"SDL_CreateWindow init error: %s\n", SDL_GetError());
		return 1;
	}

	// Set OpenGL context parameters
	int major = 3, minor = 2;
	SDL_Log("Asking for OpenGL %d.%d context\n", major, minor);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_PROFILE_CORE); 
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 

	// OpenGL Context Creation
	glcontext = SDL_GL_CreateContext(win);
	if (glcontext == NULL) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			"SDL_GL_CreateContext init error: %s\n", SDL_GetError());
		return 1;
	}
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
	SDL_Log("Got an OpenGL %d.%d context\n", major, minor);

	// initialise GLEW - sets up the OpenGL function pointers for the version of OpenGL we're using
	GLenum rev;
	glewExperimental = GL_TRUE; //GLEW isn't perfect - see https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
	rev = glewInit();
	if (GLEW_OK != rev) {
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			"glewInit error: %s\n", glewGetErrorString(rev));
		return 1;
	}

	Shader modelShader(getFullPath("assets/Shaders/VertexShader.txt"), getFullPath("assets/Shaders/FragmentShader.txt"));
	Shader cubeShader(getFullPath("assets/Shaders/CubeMapVertexShader.txt"), getFullPath("assets/Shaders/CubeMapFragmentShader.txt"));


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);	
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// ***** Build and compile our shader program *****
	viewPos.x = camPos.x;
	viewPos.y = camPos.y;
	viewPos.z = camPos.z;
	projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);	/*	fovy, aspect, near, far		*/
	//viewMatrix = glm::lookAt(camPos, camTarget, glm::vec3(0.0f, 1.0f, 0.0f));	
	//viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -10.0f));

	//projection init
	setWindowSize();
	

	SetUp();

	double deltaTime = 0, playerMoveDis = 0; //time difference
	chrono::high_resolution_clock::time_point previous = getCurrentTime();

	while (running)
	{
		deltaTime = duration_cast<microseconds>(getCurrentTime() - previous).count() / 1000000.0;
		previous = getCurrentTime();

		processInput();
		update(deltaTime);
		render(projectionMatrix, modelShader, cubeShader);
	}

	// Clean up
	SDL_Log("Finished. Cleaning up and closing down\n");

	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
	return 0;
}

void processInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
					setWindowSize();
				}
				break;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						running = false;
						break;
					case SDLK_a :
						gameWorld.playerMoveSpeed = -4;
						break;

					case SDLK_d:
						gameWorld.playerMoveSpeed = 4;
						break;
					case SDLK_o:
						toggleFullScreen();
						break;
				}
			break;

			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
				case SDLK_a:
					gameWorld.playerMoveSpeed = 0;
					break;
				case SDLK_d:
					gameWorld.playerMoveSpeed = 0;
					break;
				}
			break;

			case SDL_QUIT:
				running = false;
			break;
		}
	}
}

void update(double &deltaTime)
{
	//firstModel.Rotate(40.0f * deltaTime, 0.0f, 40.0f * deltaTime);

	//camPos.z = camPos.z + deltaTime * -2.0f;
	viewPos.x = camPos.x;
	viewPos.y = camPos.y;
	viewPos.z = camPos.z;
	//viewRot.y += deltaTime * 10.0f;
	//viewMatrix = glm::lookAt(camPos, camTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	//viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, deltaTime * 40.0f));
	//viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, -deltaTime * 0.1f, 0.0f));
	//gameWorld.player.Move(10.0f * deltaTime, 0.0f, 0.0f);

	bool decend = false;

	gameWorld.player.Move(gameWorld.playerMoveSpeed * deltaTime, 0.0f, 0.0f);

	if (gameWorld.enemies.size() == 0) {
		//reset();
	}
	for (int i = 0; i < gameWorld.enemies.size(); i++)
	{
		//Alien Movement
		if (gameWorld.right) {
			gameWorld.enemies.at(i).Move(gameWorld.alienMoveSpeed * deltaTime, 0.0f, 0.0f);
		}
		else {
			gameWorld.enemies.at(i).Move(-gameWorld.alienMoveSpeed * deltaTime, 0.0f, 0.0f);
		}
		//Alien kill check
		if (!gameWorld.enemies.at(i).isAlive) {
			//Spin off code
			gameWorld.enemies.at(i).dyingTimer += deltaTime;
			gameWorld.enemies.at(i).Move(0, deltaTime * 0.5f, 0);
			gameWorld.enemies.at(i).Rotate(deltaTime * -75.0f, deltaTime * -75.0f, deltaTime * -75.0f);
			gameWorld.enemies.at(i).Scale(deltaTime * -0.75f);
			if (gameWorld.enemies.at(i).dyingTimer >= 1.5f) gameWorld.enemies.at(i).Remove(true);
		}
	}
	if (gameWorld.right)	gameWorld.alienXOffset += gameWorld.alienMoveSpeed * deltaTime;
	else gameWorld.alienXOffset -= gameWorld.alienMoveSpeed * deltaTime;

	//decend check
	if (gameWorld.alienXOffset > 8.5f && gameWorld.right) {
		decend = true;
		gameWorld.enemyShootPermission = true;
	}
	if (gameWorld.alienXOffset < 0.0f && !gameWorld.right) {
		decend = true;
	}

	if (decend) {
		gameWorld.right = gameWorld.right ? false : true;
		for (int i = 0; i < gameWorld.enemies.size(); i++) {
			gameWorld.enemies.at(i).Move(0.0f, gameWorld.Ygap / 2, 0.0f);
			gameWorld.alienYOffset += gameWorld.Ygap / 2;
			gameWorld.alienMoveSpeed += 0.02;
		}
	}
	if (gameWorld.alienYOffset == 300.0f) {
		gameWorld.playerLives--;
		//reset();
	}

	//if (gameWorld.enemyShootPermission) {
	//	int shootDieRoll = std::rand() % 300;
	//	if (shootDieRoll < gameWorld.enemies.size()) {
	//		gameWorld.EnemyShoot(gameWorld.enemies.at(shootDieRoll).x, gameWorld.enemies.at(shootDieRoll).y);	//only shoot if alive.
	//	}
	//}
}


void render(glm::mat4 &projectionMatrix, Shader modelShader, Shader cubeShader)
{
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	cubeMap.Render(cubeShader, projectionMatrix, viewPos, viewRot);
	for (int i = 0; i < gameWorld.enemies.size(); i++)
	gameWorld.enemies.at(i).Render(modelShader, projectionMatrix, viewPos, viewRot);
	gameWorld.player.Render(modelShader, projectionMatrix, viewPos, viewRot);
	
	
	SDL_GL_SwapWindow(win);
}

