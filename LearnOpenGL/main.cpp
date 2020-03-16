#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Model.h"
#include "MapGenerator.h"
#include "Spider.h"
#include "Bazooka.h"
#include "Arrow.h"
#include "Rocket.h"
#include "water.h"
#include "ParticleSys.h"
#include "GameController.h"

#include "Skybox.h"

#include <iostream>
#include <ctime>

#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//objects
Arrow arrow(SCR_WIDTH, SCR_HEIGHT);
Rocket rocket;
ParticleSys* rocketSmoke = nullptr;
ParticleSys* explosion = nullptr;
GameController* GameContr = nullptr;

int main()
{
	srand(time(NULL));	//randomowy seed przy kazdej kompilacji
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("Shaders/animated_model.vert", "Shaders/shader.frag");
	Shader mapShader("Shaders/shader.vert", "Shaders/shader.frag");
	Shader skyboxShader("Shaders/Skybox.vert", "Shaders/Skybox.frag");
	Shader particleShader("Shaders/particle.vert", "Shaders/particle.frag");

	// load models
	// -----------
	Model ourModel;
	ourModel.addTextures("resources/objects/Spider/kolorki.png", "resources/objects/Spider/kolorki_specular.png");
	ourModel.loadModel("resources/objects/Spider/pajunk_1_1_1.fbx");
	ourModel.initShaders(ourShader.ID);

	Model bazookaModel;
	bazookaModel.addTextures("resources/objects/Bazooka/bazooka_diffuse.png", "");
	bazookaModel.loadModel("resources/objects/Bazooka/bazooka.fbx");
	bazookaModel.initShaders(ourShader.ID);

	Model arrowModel;
	arrowModel.addTextures("resources/objects/arrow/blue.png", "");
	arrowModel.loadModel("resources/objects/arrow/Arrow.fbx");
	arrowModel.initShaders(ourShader.ID);

	Model rocketModel;
	rocketModel.addTextures("resources/objects/pocisk/pocisk_diffuse.png", "");
	rocketModel.loadModel("resources/objects/pocisk/pocisk.fbx");
	rocketModel.initShaders(ourShader.ID);

	int worldSize = 100;
	MapGenerator maps(worldSize, 50.0f, 13, 1, 3, 300);

	maps.loadImageToTexture("Textures/grass_new.png", "Textures/grass_new_specular.png");
	maps.initShader(mapShader);

	GameContr = new GameController(4, &maps, &ourModel, &bazookaModel);	//tu sie wszystko zaczyna

	camera.map = &maps;

	arrow.mainModel = &arrowModel;
	arrow.mainCamera = &camera;

	rocket.mainModel = &rocketModel;
	rocket.arrow = &arrow;
	rocket.map = &maps;
	rocket.gameContr = GameContr;

	rocket.setPosition(GameContr->bazookas[GameContr->getAcitveSpider()]);

	Skybox skybox(&skyboxShader);

	rocketSmoke = new ParticleSys(0.01f, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 1.5f, glm::vec3(0.0f, 1.0f, 0.0f), 0.5f, glm::vec3(0.0f, -1.0f, 0.0f), 5.0f, 3.0f, &particleShader, 8, 40);
	rocketSmoke->init("Textures/particle/smoke.png");
	rocketSmoke->isActive = false;

	rocket.smoke = rocketSmoke;	//ustawiam particle rakiety

	explosion = new ParticleSys(0.01f, glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, 2.5f, glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 0.6f, &particleShader, 4, 15);
	explosion->init("Textures/particle/explosion.png");
	explosion->isActive = false;

	rocket.explosion = explosion;

	//woda
	water watr(&mapShader, "Textures/water.jpg", "Textures/water_specular.jpg", 100.0f, worldSize / 2, 0.1f);

	// draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix(deltaTime);
		glm::mat4 model = glm::mat4(1.0f);

		ourShader.use();

		ourShader.setVec3("viewPos", camera.Position);
		ourShader.setFloat("material.shininess", 32.0f);

		glm::vec3 orange = glm::vec3(1.0f, 0.612f, 0.1f);

		// directional light
		ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		ourShader.setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
		ourShader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		ourShader.setVec3("pointLights[0].position", rocket.position + glm::vec3(0.0f, 0.5f, 0.0f));
		ourShader.setVec3("pointLights[0].ambient",	orange * 0.05f * rocket.lightStr);
		ourShader.setVec3("pointLights[0].diffuse", orange * 0.8f * rocket.lightStr);
		ourShader.setVec3("pointLights[0].specular", orange* rocket.lightStr);
		ourShader.setFloat("pointLights[0].constant", 1.0f);
		ourShader.setFloat("pointLights[0].linear", 0.007);
		ourShader.setFloat("pointLights[0].quadratic", 0.0002);

		GameContr->Draw(&ourShader, projection * view, deltaTime, &camera);

		//rakieta
		model = rocket.getModel(deltaTime, -camera.Front);

		glm::mat4 MVP = projection * view * model;
		ourShader.setMat4("MVP", MVP);
		ourShader.setMat4("M_matrix", model);
		glm::mat4 matr_normals_cube = glm::mat4(glm::transpose(glm::inverse(model)));
		ourShader.setMat4("normals_matrix", matr_normals_cube);

		rocketModel.draw(ourShader.ID);

		rocket.decLightStr(deltaTime, 2.0f);	//blysk po wybuchu

		//pozycja kamery
		camera.target = rocket.position + GameContr->spiders[GameContr->getAcitveSpider()]->upward * 0.1f;

		//strzalka
		arrow.draw(model, projection * view, &ourShader);

		mapShader.use();

		mapShader.setVec3("viewPos", camera.Position);
		mapShader.setFloat("material.shininess", 32.0f);

		// directional light
		mapShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		mapShader.setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
		mapShader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
		mapShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		mapShader.setVec3("pointLights[0].position", rocket.position + glm::vec3(0.0f, 0.5f, 0.0f));
		mapShader.setVec3("pointLights[0].ambient", orange * 0.05f * rocket.lightStr);
		mapShader.setVec3("pointLights[0].diffuse", orange * 0.8f * rocket.lightStr);
		mapShader.setVec3("pointLights[0].specular", orange * rocket.lightStr);
		mapShader.setFloat("pointLights[0].constant", 1.0f);
		mapShader.setFloat("pointLights[0].linear", 0.007);
		mapShader.setFloat("pointLights[0].quadratic", 0.0002);

		model = glm::mat4(1.0f);	//mapa ma koordynaty 0, 0 na w lewym dolnym krancu
		mapShader.setMat4("model", model);
		mapShader.setMat4("view", view);
		mapShader.setMat4("projection", projection);

		maps.Draw();

		watr.Draw(view, projection, deltaTime);

		//rysowanie skyboxa zawsze na koncu, ale przed particlami
		skybox.Draw(camera.GetViewMatrix(deltaTime), projection);

		//particle zawsze na koncu
		rocketSmoke->position = rocket.position;
		rocketSmoke->Update(deltaTime);
		rocketSmoke->Draw(view, projection, camera.Position);

		explosion->Update(deltaTime);
		explosion->Draw(view, projection, camera.Position);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow * window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
	{	//jezeli nie uzywam klawiszy do biegania to nie biegam
		for (int i = 0; i < GameContr->spiders.size(); i++)
		{
			GameContr->spiders[i]->isRunning = false;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		GameContr->spiders[GameContr->getAcitveSpider()]->move(10, deltaTime , camera.Front, camera.Right);
	else
	{
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			GameContr->spiders[GameContr->getAcitveSpider()]->move(30, deltaTime, camera.Front, camera.Right);
		else
		{
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				GameContr->spiders[GameContr->getAcitveSpider()]->move(12, deltaTime, camera.Front, camera.Right);
			else
			{
				if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
					GameContr->spiders[GameContr->getAcitveSpider()]->move(32, deltaTime, camera.Front, camera.Right);
				else
				{
					if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
						GameContr->spiders[GameContr->getAcitveSpider()]->move(0, deltaTime, camera.Front, camera.Right);
					else
					{
						if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
							GameContr->spiders[GameContr->getAcitveSpider()]->move(1, deltaTime, camera.Front, camera.Right);
						else
						{
							if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
								GameContr->spiders[GameContr->getAcitveSpider()]->move(0, deltaTime, camera.Front, camera.Right);
							if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
								GameContr->spiders[GameContr->getAcitveSpider()]->move(2, deltaTime, camera.Front, camera.Right);
							if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
								GameContr->spiders[GameContr->getAcitveSpider()]->move(3, deltaTime, camera.Front, camera.Right);
							if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
								GameContr->spiders[GameContr->getAcitveSpider()]->move(1, deltaTime, camera.Front, camera.Right);
						}
					}
				}
			}
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	arrow.scrW = width;
	arrow.scrH = height;
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)	//strzelanie
	{
		rocket.shoot(camera.Front);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

