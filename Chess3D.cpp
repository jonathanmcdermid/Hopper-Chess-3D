#include "Chess3D.h"

void Chess3D::dummyWindow()//hack to get GL version
{
	glewExperimental = true;
	if (!glfwInit())
		return;
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	GLFWwindow* window;
	window = glfwCreateWindow(100, 100, "Dummy window", NULL, NULL);
	if (window == NULL) {
		return;
	}
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
		return;

	std::string versionString = std::string((const char*)glGetString(GL_VERSION));

	std::cout <<"OPENGL VERSION:: "<< versionString << "\n";

	glfwTerminate();

	GL_VERSION_MAJOR = versionString.at(0) - '0';
	GL_VERSION_MINOR = versionString.at(2) - '0';
}

void Chess3D::initGLFW()
{
	if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "ERROR::GLFW_INIT_FAILED\n";
		glfwTerminate();
	}
}

void Chess3D::initWindow(const char* title, bool resizable)
{
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->GL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->GL_VERSION_MINOR);
	glfwWindowHint(GLFW_RESIZABLE, resizable);

	this->window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title, NULL, NULL);

	if (this->window == NULL)
	{
		std::cout << "ERROR::GLFW_WINDOW_INIT_FAILED\n";
		glfwTerminate();
	}

	glfwGetFramebufferSize(this->window, &this->framebufferWidth, &this->framebufferHeight);
	glfwSetFramebufferSizeCallback(this->window, Chess3D::framebuffer_resize_callback);
	glfwMakeContextCurrent(this->window);
}

void Chess3D::initGLEW()
{
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR::GLEW_INIT_FAILED\n";
		glfwTerminate();
	}
}

void Chess3D::initOpenGLOptions()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Chess3D::initMatrices()
{
	this->ViewMatrix = glm::mat4(1.f);
	this->ViewMatrix = glm::lookAt(this->startCamPosition, this->startCamPosition + this->startCamFront, this->startWorldUp);
	this->ProjectionMatrix = glm::mat4(1.f);
	this->ProjectionMatrix = glm::perspective(
		glm::radians(this->fov),
		static_cast<float>(this->framebufferWidth) / this->framebufferHeight,
		this->nearPlane,
		this->farPlane
	);
}

void Chess3D::initShaders()
{
	this->shaders.push_back(new Shader(this->GL_VERSION_MAJOR, this->GL_VERSION_MINOR, (char*)"shaders/vertex_core.glsl", (char*)"shaders/fragment_core.glsl"));
	this->shaders.push_back(new Shader(this->GL_VERSION_MAJOR, this->GL_VERSION_MINOR, (char*)"shaders/text.vs", (char*)"shaders/text.fs"));
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT));
	shaders[SHADER_TEXT]->use();
	glUniformMatrix4fv(glGetUniformLocation(shaders[SHADER_TEXT]->getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void Chess3D::initTextures()
{
	this->textures.push_back(new Texture("images/PawnDiffuse.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("images/KnightDiffuse.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("images/BishopDiffuse.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("images/RookDiffuse.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("images/QueenDiffuse.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("images/KingDiffuse.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("images/b_tile.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("images/w_tile.png", GL_TEXTURE_2D));
	this->textures.push_back(new Texture("images/highlight_tile.png", GL_TEXTURE_2D));
}

void Chess3D::initTexts()
{
	this->texts.push_back(new Text((char*) "fonts/consola.ttf"));
}

void Chess3D::initMaterials()
{
	for (int i = 0; i < SPACES; ++i)
	{
		if ((i + i / WIDTH) % 2)
			this->materials.push_back(new Material(glm::vec3(0.1f), glm::vec3(1.3f), glm::vec3(0.f), 0, 1));
		else
			this->materials.push_back(new Material(glm::vec3(0.1f), glm::vec3(1.3f), glm::vec3(0.f), 0, 1));
	}
	this->materials.push_back(new Material(glm::vec3(0.05f), glm::vec3(0.4f), glm::vec3(1.f), 1, 1));
	this->materials.push_back(new Material(glm::vec3(0.5f), glm::vec3(1.f), glm::vec3(1.f), 1, 1));

}

void Chess3D::initModels()
{
	std::vector<Mesh*>tile_mesh;

	tile_mesh.push_back(
		new Mesh(
			&Quad(),
			glm::vec3(1.f, 0.f, 0.f),
			glm::vec3(0.f, 0.f, 0.f),
			glm::vec3(0.f),
			glm::vec3(1.f)));

	for (int i = 0; i < SPACES; ++i) 
	{
		if ((i + i / WIDTH) % 2) 
		{
			this->models.push_back(new Model(
				glm::vec3((float)(i % WIDTH), 0.f, (float)(i / WIDTH)),
				this->materials[i],
				this->textures[TEXTURE_TILE_BLACK],
				this->textures[TEXTURE_TILE_HIGHLIGHT],
				tile_mesh));
		}
		else 
		{
			this->models.push_back(new Model(
				glm::vec3((float)(i % WIDTH), 0.f, (float)(i / WIDTH)),
				this->materials[i],
				this->textures[TEXTURE_TILE_WHITE],
				this->textures[TEXTURE_TILE_HIGHLIGHT],
				tile_mesh));
		}
	}

	std::string fString;
	int index;

	if (this->ifx == nullptr)
	{
		std::cout << "ERROR::INTERFACE NOT INITIALIZED\n";
		glfwTerminate();
		return;
	}

	for (int i = 0; i < SPACES; ++i) 
	{
		switch (abs(this->ifx->game.grid[i])) 
		{
		case W_PAWN:
			fString = "objects/pawn.obj";
			index = PINDEX;
			break;
		case W_KNIGHT:
			fString = "objects/knight.obj";
			index = NINDEX;
			break;
		case W_BISHOP:
			fString = "objects/bishop.obj";
			index = BINDEX;
			break;
		case W_ROOK:
			fString = "objects/rook.obj";
			index = RINDEX;
			break;
		case W_QUEEN:
			fString = "objects/queen.obj";
			index = QINDEX;
			break;
		case W_KING:
			fString = "objects/king.obj";
			index = KINDEX;
			break;
		default:
			continue;
		}
		this->models.push_back(new Model(
			glm::vec3((float)(i % WIDTH), 0.f, (float)(i / WIDTH)),
			(this->ifx->game.grid[i] > 0) ? glm::vec3(0.f) : glm::vec3(180.f, 0, 180.f),
			this->materials[MATERIAL_PIECE_BLACK + (int) (this->ifx->game.grid[i] > 0)],
			this->textures[index],
			this->textures[index],
			fString.c_str()));
	}

	for (auto*& i : tile_mesh)
		delete i;
}

void Chess3D::initLights()
{
	this->lights.push_back(new glm::vec3(4.5f, 5.f, 4.f));
}

void Chess3D::initUniforms()
{
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(ViewMatrix, "ViewMatrix");
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(ProjectionMatrix, "ProjectionMatrix");
	this->shaders[SHADER_CORE_PROGRAM]->setVec3f(*this->lights[0], "lightPos0");
}

void Chess3D::updateUniforms()
{
	this->ViewMatrix = this->camera.getViewMatrix();
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ViewMatrix, "ViewMatrix");
	this->shaders[SHADER_CORE_PROGRAM]->setVec3f(this->camera.getPosition(), "cameraPos");
	this->shaders[SHADER_CORE_PROGRAM]->setVec3f(*this->lights[0], "lightPos0");

	glfwGetFramebufferSize(this->window, &this->framebufferWidth, &this->framebufferHeight);

	this->ProjectionMatrix = glm::perspective(
		glm::radians(this->fov),
		static_cast<float>(this->framebufferWidth) / this->framebufferHeight,
		this->nearPlane,
		this->farPlane);

	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ProjectionMatrix, "ProjectionMatrix");
}

Chess3D::Chess3D(
	int argc, char* argv[],
	Hopper::Interface* i,
	const char* title,
	const int WINDOW_WIDTH, const int WINDOW_HEIGHT,
	bool resizable)
	:
	WINDOW_WIDTH(WINDOW_WIDTH),
	WINDOW_HEIGHT(WINDOW_HEIGHT),
	camera(glm::vec3(4.5f, 11.5f, 15.f), glm::vec3(0.f, 1.f, 0.f))
{
	this->ifx = i;
	this->window = nullptr;
	this->framebufferWidth = this->WINDOW_WIDTH;
	this->framebufferHeight = this->WINDOW_HEIGHT;

	this->startCamPosition = glm::vec3(4.5f, 4.5f, 12.f);
	this->startWorldUp = glm::vec3(0.f, 1.f, 0.f);
	this->startCamFront = glm::vec3(0.f, -1.f, -1.f);

	this->fov = 25.f;
	this->nearPlane = 0.1f;
	this->farPlane = 100.f;

	this->fps = 0.f;
	this->dt = 0.f;
	this->curTime = 0.f;
	this->lastTime = 0.f;
	this->secondCount = 0.f;
	this->frameCount = 0;

	this->lastMouseX = 0.0;
	this->lastMouseY = 0.0;
	this->mouseX = 0.0;
	this->mouseY = 0.0;
	this->mouseOffsetX = 0.0;
	this->mouseOffsetY = 0.0;
	this->firstMouse = true;
	this->cameraMode = false;
	this->showInfo = false;
	this->gameOver = false;
	this->moveString = "";

	this->dummyWindow();
	this->initGLFW();
	this->initWindow(title, resizable);
	this->initGLEW();
	this->initOpenGLOptions();
	this->initMatrices();
	this->initShaders();
	this->initTextures();
	this->initTexts();
	this->initMaterials();
	this->initModels();
	this->initLights();
	this->initUniforms();
}

Chess3D::~Chess3D()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();

	for (size_t i = 0; i < this->shaders.size(); ++i)
		delete this->shaders[i];
	for (size_t i = 0; i < this->textures.size(); ++i)
		delete this->textures[i];
	for (size_t i = 0; i < this->texts.size(); ++i)
		delete this->texts[i];
	for (size_t i = 0; i < this->materials.size(); ++i)
		delete this->materials[i];
	for (size_t i = 0; i < this->lights.size(); ++i)
		delete this->lights[i];
	for (auto*& i : this->models)
		delete i;
}

int Chess3D::getWindowShouldClose()
{
	return glfwWindowShouldClose(this->window);
}

void Chess3D::setWindowShouldClose()
{
	glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}

void Chess3D::updateDt()
{
	this->curTime = static_cast<float>(glfwGetTime());
	this->dt = this->curTime - this->lastTime;
	this->lastTime = this->curTime;
	this->secondCount += this->dt;
	if (this->secondCount >= 1.f) 
	{
		this->fps = this->frameCount/this->secondCount;
		this->secondCount -= 1.f;
		this->frameCount = 0;
	}
	++this->frameCount;
}

void Chess3D::updateMouseInput()
{
	glfwGetCursorPos(this->window, &this->mouseX, &this->mouseY);

	if (this->firstMouse)
	{
		this->lastMouseX = this->mouseX;
		this->lastMouseY = this->mouseY;
		this->firstMouse = false;
	}

	this->mouseOffsetX = this->mouseX - this->lastMouseX;
	this->mouseOffsetY = this->lastMouseY - this->mouseY;

	this->lastMouseX = this->mouseX;
	this->lastMouseY = this->mouseY;

	if(glfwGetInputMode(this->window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		this->camera.updateInput(dt, -1, this->mouseOffsetX * this->fov / 90, this->mouseOffsetY * this->fov / 90);
}

void Chess3D::updateKeyboardInput()
{
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->window, GLFW_TRUE);

	if (glfwGetKey(this->window, GLFW_KEY_P) == GLFW_PRESS)
		glfwSetInputMode(this->window, GLFW_CURSOR,GLFW_CURSOR_NORMAL);

	if (glfwGetKey(this->window, GLFW_KEY_O) == GLFW_PRESS)
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	this->showInfo = (glfwGetKey(this->window, GLFW_KEY_I) == GLFW_PRESS);

	if (this->cameraMode)
	{
		if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
			this->camera.Move(this->dt, FORWARD);
		if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
			this->camera.Move(this->dt, BACKWARD);
		if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
			this->camera.Move(this->dt, LEFT);
		if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
			this->camera.Move(this->dt, RIGHT);
		if (glfwGetKey(this->window, GLFW_KEY_E) == GLFW_PRESS)
			this->camera.Move(this->dt, UP);
		if (glfwGetKey(this->window, GLFW_KEY_Q) == GLFW_PRESS)
			this->camera.Move(this->dt, DOWN);
		if (glfwGetKey(this->window, GLFW_KEY_MINUS) == GLFW_PRESS)
			this->fov -= 0.1f;
		if (glfwGetKey(this->window, GLFW_KEY_EQUAL) == GLFW_PRESS)
			this->fov += 0.1f;
		if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS)
		{
			this->cameraMode = false;
			selectTiles(true);
			this->moveString = "";
		}
	}
	else
	{
		if (glfwGetKey(this->window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
		{
			while (this->moveString.length()) 
			{
				selectTiles(true);
				this->moveString.pop_back();
			}
			this->cameraMode = true;
		}
		else if (glfwGetKey(this->window, GLFW_KEY_ENTER) == GLFW_PRESS)
		{
			if(this->ifx->playerMove(this->moveString))
				initMove(this->ifx->game.getCurrM().getFrom(), this->ifx->game.getCurrM().getTo());
			while (this->moveString.length()) 
			{
				selectTiles(true);
				this->moveString.pop_back();
			}
		}
		else if (!(this->moveString.length() % 2)) {
			if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
				this->moveString += 'a';
			else if (glfwGetKey(this->window, GLFW_KEY_B) == GLFW_PRESS)
				this->moveString += 'b';
			else if (glfwGetKey(this->window, GLFW_KEY_C) == GLFW_PRESS)
				this->moveString += 'c';
			else if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
				this->moveString += 'd';
			else if (glfwGetKey(this->window, GLFW_KEY_E) == GLFW_PRESS)
				this->moveString += 'e';
			else if (glfwGetKey(this->window, GLFW_KEY_F) == GLFW_PRESS)
				this->moveString += 'f';
			else if (glfwGetKey(this->window, GLFW_KEY_G) == GLFW_PRESS)
				this->moveString += 'g';
			else if (glfwGetKey(this->window, GLFW_KEY_H) == GLFW_PRESS)
				this->moveString += 'h';
			else if (glfwGetKey(this->window, GLFW_KEY_N) == GLFW_PRESS && this->moveString.length() == 4)
				this->moveString += 'n';
			else if (glfwGetKey(this->window, GLFW_KEY_R) == GLFW_PRESS && this->moveString.length() == 4)
				this->moveString += 'r';
			else if (glfwGetKey(this->window, GLFW_KEY_Q) == GLFW_PRESS && this->moveString.length() == 4)
				this->moveString += 'q';
			else
				return;
			selectTiles(false);
		}
		else {
			if (glfwGetKey(this->window, GLFW_KEY_1) == GLFW_PRESS)
				this->moveString += '1';
			else if (glfwGetKey(this->window, GLFW_KEY_2) == GLFW_PRESS)
				this->moveString += '2';
			else if (glfwGetKey(this->window, GLFW_KEY_3) == GLFW_PRESS)
				this->moveString += '3';
			else if (glfwGetKey(this->window, GLFW_KEY_4) == GLFW_PRESS)
				this->moveString += '4';
			else if (glfwGetKey(this->window, GLFW_KEY_5) == GLFW_PRESS)
				this->moveString += '5';
			else if (glfwGetKey(this->window, GLFW_KEY_6) == GLFW_PRESS)
				this->moveString += '6';
			else if (glfwGetKey(this->window, GLFW_KEY_7) == GLFW_PRESS)
				this->moveString += '7';
			else if (glfwGetKey(this->window, GLFW_KEY_8) == GLFW_PRESS)
				this->moveString += '8';
			else
				return;
			selectTiles(false);
		}
	}
}

void Chess3D::selectTiles(bool undo)
{
	int i;
	int shift = 1;
	switch (this->moveString.length())
	{
	case 1:
		/*for (i = this->moveString.at(0) - 'a'; i < SPACES; i += WIDTH)
			this->materials[i]->diffuseTex += (undo) ? -shift : shift;*/
		break;
	case 2:
		/*for (i = this->moveString.at(0) - 'a'; i < SPACES; i += WIDTH)
			this->materials[i]->diffuseTex -= (undo) ? -shift : shift;*/
		i = this->moveString.at(0) - 'a' + (8 - this->moveString.at(1) + '0') * WIDTH;
		this->materials[i]->diffuseTex += (undo) ? -shift : shift;
		break;
	case 3:
		/*for (i = this->moveString.at(2) - 'a'; i < SPACES; i += WIDTH)
			this->materials[i]->diffuseTex += (undo) ? -shift : shift;*/
		break;
	case 4:
		/*for (i = this->moveString.at(2) - 'a'; i < SPACES; i += WIDTH)
			this->materials[i]->diffuseTex -= (undo) ? -shift : shift;*/
		i = this->moveString.at(2) - 'a' + (8 - this->moveString.at(3) + '0') * WIDTH;
		this->materials[i]->diffuseTex += (undo) ? -shift : shift;
		break;
	case 5:
		i = this->moveString.at(0) - 'a' + (8 - this->moveString.at(1) + '0') * WIDTH;
		this->materials[i]->diffuseTex -= (undo) ? -shift : shift;
		i = this->moveString.at(2) - 'a' + (8 - this->moveString.at(3) + '0') * WIDTH;
		this->materials[i]->diffuseTex -= (undo) ? -shift : shift;
		break;
	}
	if (!undo)
		render();
}

void Chess3D::initMove(int from, int to)
{
	for (int i = SPACES; i < this->models.size(); ++i) 
	{
		if ((this->models[i]->getPosition().z) * 8 + this->models[i]->getPosition().x == from) 
		{
			this->trajectories[0].pModel = models[i];
			this->trajectories[0].velocity = normalize(glm::vec3(to % 8 - from % 8, 0, to / 8 - from / 8)) / 50.f;
			this->trajectories[0].frames = (int) (50 * dot(glm::vec3(to % 8 - from % 8, 0, to / 8 - from / 8), normalize(glm::vec3(to % 8 - from % 8, 0, to / 8 - from / 8))));
			break;
		}
	}
	int direction;
	switch (this->ifx->game.getCurrM().getFlags()) 
	{
	case QCASTLE:
		for (int i = SPACES; i < this->models.size(); ++i)
		{
			if ((this->models[i]->getPosition().z) * WIDTH + this->models[i]->getPosition().x == from - 4) 
			{
				this->trajectories[1].pModel = this->models[i];
				this->trajectories[1].velocity = -this->trajectories[0].velocity;
				this->trajectories[1].frames = (int) 1.5 * this->trajectories[0].frames;
				return;
			}
		}
	case KCASTLE:
		for (int i = SPACES; i < this->models.size(); ++i)
		{
			if ((this->models[i]->getPosition().z) * WIDTH + this->models[i]->getPosition().x == from + 3) 
			{
				this->trajectories[1].pModel = this->models[i];
				this->trajectories[1].velocity = -this->trajectories[0].velocity;
				this->trajectories[1].frames = this->trajectories[0].frames;
				return;
			}
		}
	case ENPASSANT:
		direction = (this->ifx->game.turn) ? BOARD_NORTH : BOARD_SOUTH;
		for (int i = SPACES; i < this->models.size(); ++i)
		{
			if ((this->models[i]->getPosition().z) * WIDTH + models[i]->getPosition().x == to + direction) 
			{
				this->trajectories[1].pModel = this->models[i];
				this->trajectories[1].velocity = glm::vec3(0, -1, 0) / 50.f;
				this->trajectories[1].frames = 100;
				return;
			}
		}
	case CAPTURE:
	case NPROMOTEC:
	case BPROMOTEC:
	case RPROMOTEC:
	case QPROMOTEC:
		for (int i = SPACES; i < this->models.size(); ++i)
		{
			if ((this->models[i]->getPosition().z) * WIDTH + this->models[i]->getPosition().x == to) 
			{
				this->trajectories[1].pModel = this->models[i];
				this->trajectories[1].velocity = glm::vec3(0, -1, 0) / 50.f;
				this->trajectories[1].frames = 100;
				return;
			}
		}
	default:
		this->trajectories[1].pModel = this->models[0];
		this->trajectories[1].velocity = glm::vec3(0, 0, 0);
		this->trajectories[1].frames = 0;
	}
}

void Chess3D::finalizeMove()
{
	if (ifx->game.isCheckMate())
		this->gameOver = true;
	switch (ifx->game.getCurrM().getFlags()) 
	{
	case ENPASSANT:
	case CAPTURE:
		*this->trajectories[1].pModel = *this->models[this->models.size() - 1];
		this->models.pop_back();
		break;
	case NPROMOTEC:
		*this->trajectories[1].pModel = *this->models[this->models.size() - 1];
		this->models.pop_back();
		this->trajectories[0].pModel->update("objects/knight.obj");
		break;
	case BPROMOTEC:
		*this->trajectories[1].pModel = *this->models[this->models.size() - 1];
		this->models.pop_back();
		this->trajectories[0].pModel->update("objects/bishop.obj");
		break;
	case RPROMOTEC:
		*this->trajectories[1].pModel = *this->models[this->models.size() - 1];
		this->models.pop_back();
		this->trajectories[0].pModel->update("objects/rook.obj");
		break;
	case QPROMOTEC:
		*this->trajectories[1].pModel = *this->models[this->models.size() - 1];
		this->models.pop_back();
		this->trajectories[0].pModel->update("objects/queen.obj");
		break;
	case NPROMOTE:
		this->trajectories[0].pModel->update("objects/knight.obj");
		break;
	case BPROMOTE:
		this->trajectories[0].pModel->update("objects/bishop.obj");
		break;
	case RPROMOTE:
		this->trajectories[0].pModel->update("objects/rook.obj");
		break;
	case QPROMOTE:
		this->trajectories[0].pModel->update("objects/queen.obj");
		break;
	}
	for (int i = 0; i < 2; ++i)
	{
		this->trajectories[i].pModel->setPosition(glm::vec3(
			round(this->trajectories[i].pModel->getPosition().x),
			round(this->trajectories[i].pModel->getPosition().y),
			round(this->trajectories[i].pModel->getPosition().z)));
	}
}

void Chess3D::updateInput()
{
	glfwPollEvents();
	this->updateKeyboardInput();
	this->updateMouseInput();
}

void Chess3D::update()
{
	this->updateDt();

	if (this->trajectories[0].frames || this->trajectories[1].frames)
	{
		if (this->trajectories[0].frames)
		{
			this->trajectories[0].pModel->move(this->trajectories[0].velocity);
			--this->trajectories[0].frames;
		}
		if (this->trajectories[1].frames)
		{
			this->trajectories[1].pModel->move(this->trajectories[1].velocity);
			--this->trajectories[1].frames;
		}
		if (!this->trajectories[0].frames && !this->trajectories[1].frames)
			this->finalizeMove();
	}
	else
	{
		if (this->ifx->game.turn)
			this->updateInput();
		else
		{
			this->ifx->botMove();
			this->initMove(this->ifx->game.getCurrM().getFrom(), this->ifx->game.getCurrM().getTo());
		}
	}
}

void Chess3D::displayInfo()
{
	this->texts[0]->RenderText(
		*this->shaders[SHADER_TEXT], "Hopper Chess 3D",								
		WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.90f, 0.6f, glm::vec3(1.f, 1.f, 1.f));
	this->texts[0]->RenderText(
		*this->shaders[SHADER_TEXT], "FPS: " + std::to_string(this->fps),
		WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.88f, 0.6f, glm::vec3(1.f, 1.f, 1.f));
	this->texts[0]->RenderText(
		*this->shaders[SHADER_TEXT], "FOV: " + std::to_string(this->fov),
		WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.86f, 0.6f, glm::vec3(1.f, 1.f, 1.f));
	this->texts[0]->RenderText(
		*this->shaders[SHADER_TEXT], "X: " + std::to_string(this->camera.getPosition().x),	
		WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.84f, 0.6f, glm::vec3(1.f, 1.f, 1.f));
	this->texts[0]->RenderText(
		*this->shaders[SHADER_TEXT], "Y: " + std::to_string(this->camera.getPosition().y),	
		WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.82f, 0.6f, glm::vec3(1.f, 1.f, 1.f));
	this->texts[0]->RenderText(
		*this->shaders[SHADER_TEXT], "Z: " + std::to_string(this->camera.getPosition().z),	
		WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.80f, 0.6f, glm::vec3(1.f, 1.f, 1.f));
}

void Chess3D::displayGameOver()
{
	if (this->ifx->game.threatened[BLACK][this->ifx->game.kpos[WHITE]])
		this->texts[0]->RenderText(*this->shaders[SHADER_TEXT], "BLACK MATES",
			WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.90f, 1.f, glm::vec3(1.f, 1.f, 1.f));
	else if (this->ifx->game.threatened[WHITE][this->ifx->game.kpos[BLACK]])
		this->texts[0]->RenderText(*this->shaders[SHADER_TEXT], "WHITE MATES",
			WINDOW_WIDTH * 0.05f, WINDOW_HEIGHT * 0.90f, 1.f, glm::vec3(1.f, 1.f, 1.f));
	else
		this->texts[0]->RenderText(*this->shaders[SHADER_TEXT], "DRAW",
			WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f, 1.f, glm::vec3(1.f, 1.f, 1.f));
}

void Chess3D::render()
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	if (this->showInfo)
		this->displayInfo();

	if (this->gameOver)
		this->displayGameOver();
	this->updateUniforms();
	for (int i = 0 ; i < this->models.size(); ++i)
		this->models[i]->render(this->shaders[SHADER_CORE_PROGRAM]);
	glfwSwapBuffers(this->window);
	glFlush();
	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Chess3D::framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH)
{
	glViewport(0, 0, fbW, fbH);
};

