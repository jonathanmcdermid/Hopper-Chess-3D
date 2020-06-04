#pragma once

#include "libs.h"

enum shader_enum { SHADER_CORE_PROGRAM = 0, SHADER_TEXT };
enum material_enum { MATERIAL_PIECE_BLACK = 64, MATERIAL_PIECE_WHITE};
enum texture_enum { TEXTURE_PIECE = 0, TEXTURE_TILE_BLACK = 6, TEXTURE_TILE_WHITE, TEXTURE_TILE_HIGHLIGHT};

class Chess3D
{
	class Interface;

	struct Trajectory {
		Model* pModel;
		glm::vec3 velocity;
		unsigned int frames = 0;
	};

private:

	GLFWwindow* window;
	Hopper::Interface* ifx;

	const int WINDOW_WIDTH;
	const int WINDOW_HEIGHT;
	int GL_VERSION_MAJOR;
	int GL_VERSION_MINOR;
	int framebufferWidth;
	int framebufferHeight;
	float dt;
	float curTime;
	float lastTime;
	int frameCount;
	float secondCount;
	float fps;
	double lastMouseX;
	double lastMouseY;
	double mouseX;
	double mouseY;
	double mouseOffsetX;
	double mouseOffsetY;
	bool firstMouse;
	bool cameraMode;
	bool showInfo;
	bool gameOver;

	std::string moveString;
	Camera camera;
	Trajectory trajectories[2];
	glm::mat4 ViewMatrix;
	glm::vec3 startCamPosition;
	glm::vec3 startWorldUp;
	glm::vec3 startCamFront;
	glm::mat4 ProjectionMatrix;

	float fov;
	float nearPlane;
	float farPlane;

	std::vector<Shader*> shaders;
	std::vector<Texture*> textures;
	std::vector<Material*> materials;
	std::vector<Model*> models;
	std::vector<Text*> texts;
	std::vector<glm::vec3*> lights;

	void initGLFW();
	void initWindow(const char* title, bool resizable);
	void initGLEW();
	void initOpenGLOptions();
	void initMatrices();
	void initShaders();
	void initTextures();
	void initTexts();
	void initMaterials();
	void initModels();
	void initLights();
	void initUniforms();

	void updateUniforms();
	void initMove(int from, int to);
	void finalizeMove();
	void selectTiles(bool undo);
	void displayGameOver();

	void dummyWindow();

public:

	Chess3D(
		int argc, char* argv[],
		Hopper::Interface* i,
		const char* title,
		const int WINDOW_WIDTH, const int WINDOW_HEIGHT,
		bool resizable
	);

	virtual ~Chess3D();

	int getWindowShouldClose();
	void setWindowShouldClose();
	void updateDt();
	void updateMouseInput();
	void updateKeyboardInput();
	void updateInput();
	void displayInfo();
	void update();
	void render();
	static void framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH);
};